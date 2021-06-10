#include "so_thread.h"

/* initialize a mutex */
SO_BOOL so_mutex_init(so_mutex_t *mutex)
{
#ifdef STATIC_MUTEX
	*mutex = (so_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	return TRUE;
#else
	int rc;

	rc = pthread_mutex_init(mutex, NULL);
	return rc;
#endif
}

/* destroys a mutex */
SO_BOOL so_mutex_destroy(__attribute__((unused)) so_mutex_t *mutex)
{
#ifdef STATIC_MUTEX
	return TRUE;
#else
	int rc;

	rc = pthread_mutex_destroy(mutex);
	DIE(rc != 0, "so_mutex_destroy");
	return TRUE;
#endif
}

/* wait for a mutex lock */
SO_BOOL so_mutex_lock(so_mutex_t *mutex)
{
	int rc;

	rc = pthread_mutex_lock(mutex);
	return rc;
}

/* wait for a mutex unlock */
SO_BOOL so_mutex_unlock(so_mutex_t *mutex)
{
	int rc;

	rc = pthread_mutex_unlock(mutex);
	return rc;
}

/* initialize a spinlock */
SO_BOOL so_spinlock_init(so_spinlock_t *spinlock)
{
	int rc;

	rc = pthread_spin_init(spinlock, SHARE_THREADS);
	return rc;
}

/* destroys a spinlock */
SO_BOOL so_spinlock_destroy(so_spinlock_t *spinlock)
{
	int rc;

	rc = pthread_spin_destroy(spinlock);
	return rc;
}

/* locks a spinlock */
SO_BOOL so_spinlock_lock(so_spinlock_t *spin)
{
	int rc;

	rc = pthread_spin_lock(spin);
	return rc;
}

/* unlocks a spinlock */
SO_BOOL so_spinlock_unlock(so_spinlock_t *spin)
{
	int rc;

	rc = pthread_spin_unlock(spin);
	return rc;
}

/* initialize a condition variable */
SO_BOOL so_condition_init(so_cond_t *cond)
{
	*cond = (so_cond_t)PTHREAD_COND_INITIALIZER;
	return TRUE;
}

/* wait on a certain condition, and releasing the mutex */
SO_BOOL so_condition_wait(so_cond_t *cond, so_mutex_t *mutex)
{
	int rc;

	rc = pthread_cond_wait(cond, mutex);
	return rc;
}

/* notify all threads waiting on specific event */
SO_BOOL so_condition_notify_all(so_cond_t *cond)
{
	int rc;

	rc = pthread_cond_broadcast(cond);
	return rc;
}

/* notify one thread waiting on specific event */
SO_BOOL so_condition_notify(so_cond_t *cond)
{
	int rc;

	rc = pthread_cond_signal(cond);
	return rc;
}

/* initialize a semaphore */
SO_BOOL so_semaphore_init(so_sem_t *sem, int sem_value)
{
	int rc;

	rc = sem_init(sem, SHARE_THREADS, sem_value);
	return rc;
}

/* acquire a semaphore */
SO_BOOL so_semaphore_acquire(so_sem_t *sem)
{
	int rc;

	rc = sem_wait(sem);
	return rc;
}

/* release a semaphore */
SO_BOOL so_semaphore_release(so_sem_t *sem)
{
	int rc;

	rc = sem_post(sem);
	return rc;
}

/* destroy a semaphore */
SO_BOOL so_semaphore_destroy(so_sem_t *sem)
{
	int rc;

	rc = sem_destroy(sem);
	return rc;
}

/* create and starts a thread*/
SO_BOOL so_create_thread(tid_t *thread,
					void* (*routine)(void *), void *arg)
{
	int rc;

	rc = pthread_create(thread, NULL, routine, arg);
	return rc;
}

/* join a thread */
SO_BOOL so_join_thread(tid_t thread)
{
	int rc;

	rc = pthread_join(thread, NULL);
	return rc;
}
