#include "so_scheduler.h"
#include "utils.h"
#include "comparators.h"
#include <string.h>

static so_scheduler_t so_scheduler;
static unsigned long timestamp;

/* reschedule function after round robin algorithm */
static void reschedule(void)
{
	so_thread_t *running_thread;
	so_thread_t **front_thread_adddress;
	so_thread_t *front_thread;
	so_thread_t *preempted_thread = NULL;
	priority_queue_t *pq;
	size_t pq_size;

	LOCK(so_scheduler);

	running_thread = so_scheduler.running_thread;
	pq = so_scheduler.pq;
	pq_size = priority_queue_size(pq);

	/** if one of the following 2 condition happen:
	 * 1. no thread is running
	 * 2. running thread has terminated
	 * choose the next thread to schedule as top of the pq
	 * if pq is empty, then signalize so_end that all thread have finished
	 */
	if (running_thread == NULL || running_thread->status == TERMINATED) {
		if (pq_size == 0) {
			if (so_scheduler.num_active_threads == 0) {
				so_condition_notify(&so_scheduler.finish_cond);
				UNLOCK(so_scheduler);
				return;
			}
			so_scheduler.running_thread = NULL;
		} else {
			front_thread_adddress =
					(so_thread_t **) priority_queue_top(pq);

			front_thread = *front_thread_adddress;
			priority_queue_pop(pq);
			running_thread = front_thread;
			so_scheduler.running_thread = front_thread;
			so_scheduler.running_thread->status = RUNNING;
			SCHEDULE_THREAD(running_thread);
		}
		/** if running thread is waiting for a I/O device,
		 *  it needs to be preempted and other thread should run
		 *  until the current thread is signalized and added back
		 *  to the priority queue.
		 */
	} else if (running_thread->status == WAITING) {
		preempted_thread = so_scheduler.running_thread;
		if (pq_size == 0)
			so_scheduler.running_thread = NULL;
		else {
			front_thread_adddress =
					(so_thread_t **)priority_queue_top(pq);
			front_thread = *front_thread_adddress;
			priority_queue_pop(pq);
			running_thread = front_thread;
			so_scheduler.running_thread = front_thread;
			so_scheduler.running_thread->status = RUNNING;
			SCHEDULE_THREAD(running_thread);
		}
		/** if the first two if clauses weren't matched, then
		 * the thread status is ready and its quantum has finished.
		 * If there is a best option, preempt this thread and schedule
		 * the other one. Otherwise, reset the time quantum for this
		 * thread
		 */
	} else {
		if (pq_size == 0) {
			if (running_thread->remaining_time == 0) {
				running_thread->remaining_time =
							so_scheduler.q_time;

				running_thread->thread_timestamp =
							++timestamp;
			}
		} else {
			front_thread_adddress =
					(so_thread_t **) priority_queue_top(pq);

			front_thread = *front_thread_adddress;
			if ((running_thread->remaining_time == 0 &&
				running_thread->arg.priority ==
				front_thread->arg.priority) ||

				(running_thread->arg.priority <
				front_thread->arg.priority)) {

				priority_queue_pop(pq);
				preempted_thread = running_thread;
				preempted_thread->remaining_time =
						so_scheduler.q_time;
				preempted_thread->thread_timestamp =
						++timestamp;

				preempted_thread->status = READY;
				priority_queue_push(pq, &preempted_thread);
				running_thread = front_thread;
				so_scheduler.running_thread = front_thread;
				so_scheduler.running_thread->status = RUNNING;
				SCHEDULE_THREAD(running_thread);
			} else if (
				running_thread->remaining_time == 0) {
				running_thread->remaining_time =
						so_scheduler.q_time;
			}
		}
	}

	UNLOCK(so_scheduler);
	if (preempted_thread)
		WAIT_FOR_SCHEDULE(preempted_thread);
}

int so_init(unsigned int q_time, unsigned int num_io_dev)
{
	int rc;
	size_t i;

	/* check the valid condition for an initialization */
	if (so_scheduler.initialized == TRUE ||
		q_time == 0 ||
		num_io_dev > SO_MAX_DEVICE)
		return SO_FAILURE;

	timestamp = 0;
	so_scheduler.num_io_devices = num_io_dev;
	so_scheduler.q_time = q_time;
	so_scheduler.initialized = TRUE;
	so_scheduler.num_active_threads = 0;
	so_scheduler.running_thread = NULL;

	/* initialize the syncronizing mechanism */
	rc = so_mutex_init(&so_scheduler.lock);
	DIE(rc != TRUE, "mutex init failed");

	rc = so_condition_init(&so_scheduler.finish_cond);
	DIE(rc != TRUE, "condition init failed");

	/* initialize the data structures for the scheduler */
	so_scheduler.pq =
		priority_queue_init(
			sizeof(so_thread_t),
			compare_so_threads,
			NULL);

	so_scheduler.terminated_threads =
				vector_init(sizeof(so_thread_t *));

	for (i = 0; i < num_io_dev; ++i)
		so_scheduler.waiting_threads_io[i] =
				vector_init(sizeof(so_thread_t));

	return SO_SUCCESS;
}

void so_end(void)
{
	so_cond_t *so_cond;
	so_mutex_t *so_mutex;
	so_vector_t *so_vector;
	size_t v_size;
	so_thread_t **thread_obj;
	size_t i;

	/* check if the so_init was called before */
	if (so_scheduler.initialized == FALSE)
		return;

	so_cond = &so_scheduler.finish_cond;
	so_mutex = &so_scheduler.lock;
	so_vector = so_scheduler.terminated_threads;

	LOCK(so_scheduler);
	/* if there are still active threads, wait for them to finish */
	while (so_scheduler.num_active_threads > 0)
		so_condition_wait(so_cond, so_mutex);

	/* release the data structure and syncronization mechanism used */
	priority_queue_free(so_scheduler.pq);
	v_size = vector_size(so_vector);
	for (i = 0; i < v_size; ++i) {
		thread_obj =
			(so_thread_t **) vector_get_back(so_vector);
		so_join_thread((*thread_obj)->thread);
		so_semaphore_destroy(&(*thread_obj)->preempted);
		free(*thread_obj);
		vector_pop_back(so_vector);
	}

	free_vector(so_scheduler.terminated_threads);
	so_mutex_destroy(&so_scheduler.lock);

	for (i = 0; i < so_scheduler.num_io_devices; ++i)
		free_vector(so_scheduler.waiting_threads_io[i]);

	UNLOCK(so_scheduler);

	/* mark the scheduler as unitialized */
	so_scheduler.initialized = FALSE;

}

void so_exec(void)
{
	so_thread_t *current_thread;

	/* check if there is a thread created by so_fork that is running */
	DIE(so_scheduler.running_thread == NULL, "no thread running");

	/* just spend time on the processor */
	current_thread = so_scheduler.running_thread;
	current_thread->remaining_time--;
	reschedule();
}

int so_wait(unsigned int io_device)
{

	so_thread_t *running_thread;
	SO_BOOL status;

	running_thread = NULL;
	status = SO_SUCCESS;

	LOCK(so_scheduler);
	DIE(so_scheduler.running_thread == NULL, "no thread running");
	running_thread = so_scheduler.running_thread;
	running_thread->remaining_time--;
	if (io_device >= so_scheduler.num_io_devices)
		status = SO_FAILURE;
	else {
		/** mark current thread as WAITING and add it in
		 * the specific I/O queue
		 */
		running_thread->status = WAITING;
		vector_push_back(
			so_scheduler.waiting_threads_io[io_device],
			&running_thread);
	}

	UNLOCK(so_scheduler);
	reschedule();
	return status;
}

int so_signal(unsigned int io_device)
{
	int i, num_threads_signal = 0;
	so_thread_t **last_thread_address;
	so_thread_t *last_thread;
	so_thread_t *running_thread;
	SO_BOOL status = SO_SUCCESS;

	LOCK(so_scheduler);
	DIE(so_scheduler.running_thread == NULL, "no thread running");

	running_thread = so_scheduler.running_thread;
	running_thread->remaining_time--;

	/* check if the device is supported by the scheduler */
	if (io_device >= so_scheduler.num_io_devices)
		status = SO_FAILURE;
	else {
		/** add all the waiting threads on that I/O device
		 * to the priority queue and mark them as READY.
		 */
		num_threads_signal =
			vector_size(
				so_scheduler.waiting_threads_io[io_device]);
		for (i = 0; i < num_threads_signal; ++i) {
			last_thread_address =
				(so_thread_t **)
				vector_get_back(
				so_scheduler.waiting_threads_io[io_device]);
			vector_pop_back(
				so_scheduler.waiting_threads_io[io_device]);
			last_thread = *last_thread_address;
			last_thread->status = READY;
			last_thread->thread_timestamp = ++timestamp;
			priority_queue_push(so_scheduler.pq, &last_thread);
		}
	}

	UNLOCK(so_scheduler);
	reschedule();
	if (status == SO_SUCCESS)
		return num_threads_signal;
	else
		return SO_FAILURE;
}

/* thread wrapper function */
void *so_start_thread(void *arg)
{
	so_thread_t *so_thread;
	so_handler handler;
	int priority;

	so_thread = (so_thread_t *)arg;
	priority = so_thread->arg.priority;
	handler = so_thread->arg.handler;


	/* block, so that no action is made until thread is schedule */
	WAIT_FOR_SCHEDULE(so_thread);
	so_thread->status = RUNNING;

	/* check the argument is properly received, by checking the priority */
	DIE(priority > SO_MAX_PRIORITY || priority < SO_MIN_PRIORITY,
										"so_priority check failed");
	/* run the function */
	handler(priority);
	LOCK(so_scheduler);

	/* mark the thread as terminated */
	so_scheduler.num_active_threads--;
	so_scheduler.num_terminated_threads++;
	vector_push_back(so_scheduler.terminated_threads, &so_thread);
	so_thread->status = TERMINATED;
	so_thread->remaining_time = 0;
	DIE(so_scheduler.running_thread->remaining_time != 0,
											"time not match");
	DIE(so_scheduler.running_thread->status != TERMINATED,
											"status not match");
	UNLOCK(so_scheduler);
	reschedule();
	return NULL;
}

tid_t so_fork(so_handler handler, unsigned int priority)
{

	so_thread_arg_t *arg;
	tid_t *thread;
	priority_queue_t *pq;
	so_thread_t *so_thread;
	so_sem_t *thread_sem;

	/* check if proper parameters were given */
	if (handler == NULL || priority > SO_MAX_PRIORITY)
		return INVALID_TID;

	so_thread = malloc(sizeof(so_thread_t));
	DIE(so_thread == NULL, "malloc failed()\n");

	/* initialize argument of the thread */
	thread = &so_thread->thread;
	arg = &so_thread->arg;
	thread_sem = &so_thread->preempted;
	so_thread->remaining_time = so_scheduler.q_time;

	so_thread->status = NEW;
	arg->handler = handler;
	arg->priority = priority;
	pq = so_scheduler.pq;

	so_semaphore_init(thread_sem, 0);
	so_create_thread(thread, so_start_thread, so_thread);
	so_thread->status = READY;

	LOCK(so_scheduler);
	/* if there was fork in another fork, spend time */
	if (so_scheduler.running_thread != NULL)
		so_scheduler.running_thread->remaining_time--;

	so_thread->thread_timestamp = ++timestamp;
	so_scheduler.num_active_threads++;
	priority_queue_push(pq, &so_thread);
	UNLOCK(so_scheduler);
	reschedule();
	return (tid_t)*thread;
}
