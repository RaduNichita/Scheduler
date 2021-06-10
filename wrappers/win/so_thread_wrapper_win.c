/* Copyright Radu Nichita radunichita99@gmail.com */
#include "so_thread.h"

/* initialize a mutex */
SO_BOOL so_mutex_init(so_mutex_t *mutex)
{

	HANDLE h;

	h = CreateMutex(
		NULL,
		FALSE,
		NULL
	);

	*mutex = h;

	return TRUE;
}

/* destroys a mutex */
SO_BOOL so_mutex_destroy(so_mutex_t *mutex)
{
	BOOL rc;

	rc = CloseHandle(*mutex);
	return rc;
}

/* wait for a mutex lock */
SO_BOOL so_mutex_lock(so_mutex_t *mutex)
{
	DWORD rc;

	rc = WaitForSingleObject(*mutex, INFINITE);
	if (rc == WAIT_FAILED)
		return FALSE;
	return TRUE;
}

/* wait for a mutex unlock */
SO_BOOL so_mutex_unlock(so_mutex_t *mutex)
{
	BOOL rc;

	rc = ReleaseMutex(*mutex);
	return rc;
}

/* not implemented */
SO_BOOL so_spinlock_init(so_spinlock_t *spinlock)
{
	return FALSE;
}

/* not implemented */
SO_BOOL so_spinlock_destroy(so_spinlock_t *spinlock)
{
	return FALSE;
}

/* not implemented */
SO_BOOL so_spinlock_lock(so_spinlock_t *spin)
{
	return FALSE;
}

/* not implemented */
SO_BOOL so_spinlock_unlock(so_spinlock_t *spin)
{
	return FALSE;
}

/* initialize a condition variable */
SO_BOOL so_condition_init(so_cond_t *cond)
{
	HANDLE h;

	h = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		NULL
	);
	*cond = h;
	return TRUE;
}

/* wait on a certain condition, and releasing the mutex */
SO_BOOL so_condition_wait(so_cond_t *cond, so_mutex_t *mutex)
{
	SignalObjectAndWait(*mutex, *cond, INFINITE, FALSE);
	return TRUE;
}

/* notify all threads waiting on specific event */
SO_BOOL so_condition_notify_all(so_cond_t *cond)
{
	PulseEvent(*cond);
	return TRUE;
}

/* notify one thread waiting on specific event */
SO_BOOL so_condition_notify(so_cond_t *cond)
{
	PulseEvent(*cond);
	return TRUE;
}

/* initialize a semaphore */
SO_BOOL so_semaphore_init(so_sem_t *sem, int sem_value)
{

	HANDLE handle;

	handle = CreateSemaphoreA(
		NULL,
		sem_value,
		sem_value + 1,
		NULL
	);

	if (handle == NULL)
		return FALSE;

	*sem = handle;
	return TRUE;
}

/* acquire a semaphore */
SO_BOOL so_semaphore_acquire(so_sem_t *sem)
{

	DWORD rc;

	rc = WaitForSingleObject(*sem, INFINITE);
	if (rc == WAIT_FAILED)
		return FALSE;

	return TRUE;
}

/* release a semaphore */
SO_BOOL so_semaphore_release(so_sem_t *sem)
{
	DWORD rc;

	rc = ReleaseSemaphore(*sem, 1, NULL);
	if (rc == 0)
		return FALSE;
	return TRUE;
}

/* destroy a semaphore */
SO_BOOL so_semaphore_destroy(so_sem_t *sem)
{
	BOOL rc;

	rc = CloseHandle(*sem);
	return rc;
}

/* create a thread and start it detached */
SO_BOOL so_create_thread(tid_t *thread,
					void* (*routine)(void *), void *arg)
{
	HANDLE h;
	BOOL rc;

	h = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE) routine,
		arg,
		0,
		thread
	);

	/* if couldn't create thread, return */
	if (h == INVALID_HANDLE_VALUE)
		return FALSE;

	/* make thread detachable */
	rc = CloseHandle(h);
	if (rc == FALSE)
		return rc;

	return TRUE;
}

/* join a thread - the function does nothing, as thread is detachable */
SO_BOOL so_join_thread(tid_t thread)
{
	return TRUE;
}
