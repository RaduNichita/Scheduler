#ifndef SO_THREAD_H_
#define SO_THREAD_H_

#if defined(__linux__)

#define TRUE 1
#define FALSE 0
#define SHARE_THREADS 0
#define SHARE_PROCESS 1
#define STATIC_MUTEX
#include <pthread.h>
#include <semaphore.h>
#define DECL_PREFIX
typedef int SO_BOOL;
typedef pthread_t tid_t;
typedef pthread_mutex_t so_mutex_t;
typedef pthread_cond_t so_cond_t;
typedef volatile int so_spinlock_t;
typedef sem_t so_sem_t;

#elif defined(_WIN32)
#include "windows.h"

#ifdef DLL_IMPORTS
#define DECL_PREFIX __declspec(dllimport)
#else
#define DECL_PREFIX __declspec(dllexport)
#endif

typedef BOOL SO_BOOL;
typedef DWORD tid_t;
typedef HANDLE so_mutex_t;
typedef HANDLE so_cond_t;
typedef HANDLE so_spinlock_t;
typedef HANDLE so_sem_t;
#else
    #error "unknown platform"
#endif

/** initialize a mutex
 * so_mutex = mutex to be initialized
 * @return TRUE if success and FALSE if problem with initializing data.
 */
SO_BOOL so_mutex_init(so_mutex_t *so_mutex);

/** destroy a mutex
 * so_mutex = mutex to be destroyed
 * @return TRUE if success and FALSE if problem with destroying data.
 */
SO_BOOL so_mutex_destroy(so_mutex_t *so_mutex);

/** lock a mutex
 * so_mutex = mutex to be locked
 * @return TRUE if the mutex was locked succesfully and FALSE otherwise
 */
SO_BOOL so_mutex_lock(so_mutex_t *so_mutex);

/** unlock a mutex
 * so_mutex = mutex to be unlocked
 * @return TRUE if the mutex was unlocked succesfully and FALSE otherwise
 */
SO_BOOL so_mutex_unlock(so_mutex_t *so_mutex);

/** initialize a spinlock
 * so_spinlock = spinlock to be initialized
 * @return TRUE if the spinlock was initialized properly and FALSE otherwise
 */
SO_BOOL so_spinlock_init(so_spinlock_t *so_spinlock);

/** destroy a spinlock
 * so_spinlock = spinlock to be destroyed
 * @return TRUE if the spinlock was destroyed properly and FALSE otherwise
 */
SO_BOOL so_spinlock_destroy(so_spinlock_t *so_spinlock);

/** lock a spinlock
 * so_spinlock = spinlock to be locked
 * @return TRUE if could acquire spinlock and FALSE otherwise.
 */
SO_BOOL so_spinlock_lock(so_spinlock_t *so_spin);

/** unlock a spinlock
 * so_spinlock = spinlock to be unlocked
 * @return TRUE if could release spinlock and FALSE otherwise.
 */
SO_BOOL so_spinlock_unlock(so_spinlock_t *so_spin);

/** initialize a conditional variable
 * so_cond = condition to be initialized.
 * @return TRUE if could initialize cond variable and FALSE otherwise
 */
SO_BOOL so_condition_init(so_cond_t *so_cond);

/** wait on a condition variable cond and release the asociated mutex
 * so_cond = condition to wait on
 * so_mutex = mutex to wait on. Need to be acquired before wait.
 * @return TRUE all the time.
 */
SO_BOOL so_condition_wait(so_cond_t *so_cond, so_mutex_t *so_mutex);

/** notify all the threads waiting on the conditional variable cond
 * so_cond = condition to be signaled
 * @return TRUE all the time
 */
SO_BOOL so_condition_notify_all(so_cond_t *so_cond);

/** notify one thread waiting on the conditional variable cond
 * so_cond = conditin to be signaled
 * @return TRUE all the time
 */
SO_BOOL so_condition_notify(so_cond_t *so_cond);

/** initialize a semaphore sem with sem_value permissions
 * so_sem = semaphore to be initialized
 * sem_value = initial number of perms of the semaphore sem
 * @return TRUE if could initialize semaphore and FALSE otherwise
 */
SO_BOOL so_semaphore_init(so_sem_t *so_sem, int sem_value);

/** acquire a semaphore -> The function blocks if there is no available
 * permission.
 * so_sem = semaphore to be acquired
 * @return TRUE all the time
 */
SO_BOOL so_semaphore_acquire(so_sem_t *so_sem);

/** release a perms on a semaphore -> call multiple time for more than
 * one release.
 * sem = semaphore to be made release on.
 * @return TRUE all the time.
 */
SO_BOOL so_semaphore_release(so_sem_t *so_sem);

/** destroy a semaphore.
 * sem = semaphore to be destroyed
 * @return TRUE all the time.
 */
SO_BOOL so_semaphore_destroy(so_sem_t *so_sem);

/** creates a new thread that is doing routine function and receives arg
 * argument
 * so_thread = the id of the newly create thread. this should be considered as
 *          an output variable
 * routine = a pointer to the function to be executed.
 * arg = argument of the function to be executed.
 * @return TRUE if thread could be created and FALSE otherwise.
 */
SO_BOOL so_create_thread(tid_t *so_thread,
					void* (*routine)(void *), void *arg);

/** joins a thread with current thread
 * thread = thread to be joined
 * @return TRUE if thread could be joined and FALSE otherwise.
 */
SO_BOOL so_join_thread(tid_t so_thread);

#endif /* _SO_THREAD_H_ */


