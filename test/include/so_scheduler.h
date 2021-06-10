#ifndef __SO_SCHEDULER_H_
#define __SO_SCHEDULER_H_

#include "so_thread.h"
#include "priority_queue.h"

#define SHARE_THREADS 0
#define SHARE_PROCESS 1


/*
 * the minimum priority that can be assigned to a thread
 */
#define SO_MIN_PRIORITY 0

/*
 * the maximum priority that can be assigned to a thread
 */
#define SO_MAX_PRIORITY 5

/*
 * the maximum number of events
 */
#define SO_MAX_DEVICE 256

/*
 * return value of failed tasks
 */
#define INVALID_TID ((tid_t)0)

#ifdef __cplusplus
extern "C" {
#endif


#define LOCK(scheduler) so_mutex_lock(&scheduler.lock)
#define UNLOCK(scheduler) so_mutex_unlock(&scheduler.lock)
#define WAIT_FOR_SCHEDULE(thread) so_semaphore_acquire(&(thread)->preempted)
#define SCHEDULE_THREAD(thread) so_semaphore_release(&(thread)->preempted)
#define SO_SUCCESS 0
#define SO_FAILURE -1

#define TRUE 1
#define FALSE 0

typedef vector_t so_vector_t;
typedef void (*so_handler)(unsigned int);


/** enum for possible threads states
 * NEW = thread was created.
 * READY = thread is ready to run
 * WAITING = thread is waiting for I/O
 * RUNNING = thread is currently running.
 * TERMINATED = thread has finished.
 */
typedef enum {
	NEW,
	READY,
	WAITING,
	RUNNING,
	TERMINATED,
} so_thread_status_t;

/** struct for keeping a thread argument
 * so_handler = pointer to the function to be executed
 * priority = priority of the thread to be executed.
 */
typedef struct {
	so_handler handler;
	unsigned int priority;
} so_thread_arg_t;

/** struct for keeping a wrapper thread.
 * thread = thread id for the current thread
 * thread_timestamp = current thread timestamp (used for adding in pq)
 * arg = wrapper for thread argument.
 * status = current status of the thread
 * remaining_time = remaining time for the current thread until preempted
 */
typedef struct {
	tid_t thread;
	unsigned long thread_timestamp;
	so_sem_t preempted;
	so_thread_arg_t arg;
	so_thread_status_t status;
	unsigned int remaining_time;
} so_thread_t;

/** struct for keeping the scheduler.
 * q_time = scheduler quantun time.
 * num_io_devices = maximum number of io devices supportted
 * initialized = variable to checker whether the scheduler has
 * been initialized.
 * pq = priority_queue for maintaing thread ordering (round-robin)
 * running_thread = thread wrapper structure of the running thread
 * num_active_thread = number of active threads
 * terminated_threads = a vector / list of terminated threads
 * lock = lock for the scheduler
 * waiting_threads = vector of threads waiting on specific I/O device
 *		waiting_threads[i] = threads waiting for the ith I/O device
 * finish_cond = conditional variable to wait for threads to complete
 */
typedef struct {
	unsigned int q_time;
	unsigned int num_io_devices;
	SO_BOOL initialized;
	priority_queue_t *pq;
	so_thread_t *running_thread;
	int num_active_threads;
	vector_t *terminated_threads;
	int num_terminated_threads;
	so_mutex_t lock;
	vector_t *waiting_threads_io[SO_MAX_DEVICE];
	so_cond_t finish_cond;
} so_scheduler_t;

/*
 * creates and initializes scheduler
 * + time quantum for each thread
 * + number of IO devices supported
 * returns: 0 on success or negative on error
 */
DECL_PREFIX int so_init(unsigned int time_quantum, unsigned int io);

/*
 * creates a new so_task_t and runs it according to the scheduler
 * + handler function
 * + priority
 * returns: tid of the new task if successful or INVALID_TID
 */
DECL_PREFIX tid_t so_fork(so_handler func, unsigned int priority);

/*
 * waits for an IO device
 * + device index
 * returns: -1 if the device does not exist or 0 on success
 */
DECL_PREFIX int so_wait(unsigned int io);

/*
 * signals an IO device
 * + device index
 * return the number of tasks woke or -1 on error
 */
DECL_PREFIX int so_signal(unsigned int io);

/*
 * does whatever operation
 */
DECL_PREFIX void so_exec(void);

/*
 * destroys a scheduler
 */
DECL_PREFIX void so_end(void);

#ifdef __cplusplus
}
#endif

#endif /* SO_SCHEDULER_H_ */

