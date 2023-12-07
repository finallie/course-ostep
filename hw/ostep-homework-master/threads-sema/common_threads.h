#ifndef __common_threads_h__
#define __common_threads_h__

#include <pthread.h>
#include <assert.h>
#include <sched.h>

#ifdef __linux__
#include <semaphore.h>
#endif

#define Pthread_create(thread, attr, start_routine, arg) assert(pthread_create(thread, attr, start_routine, arg) == 0);
#define Pthread_join(thread, value_ptr) assert(pthread_join(thread, value_ptr) == 0);

#define Pthread_mutex_init(m, v) assert(pthread_mutex_init(m, v) == 0);
#define Pthread_mutex_lock(m) assert(pthread_mutex_lock(m) == 0);
#define Pthread_mutex_unlock(m) assert(pthread_mutex_unlock(m) == 0);

#define Pthread_cond_init(cond, v) assert(pthread_cond_init(cond, v) == 0);
#define Pthread_cond_signal(cond) assert(pthread_cond_signal(cond) == 0);
#define Pthread_cond_wait(cond, mutex) assert(pthread_cond_wait(cond, mutex) == 0);

#define Mutex_init(m) assert(pthread_mutex_init(m, NULL) == 0);
#define Mutex_lock(m) assert(pthread_mutex_lock(m) == 0);
#define Mutex_unlock(m) assert(pthread_mutex_unlock(m) == 0);

#define Cond_init(cond) assert(pthread_cond_init(cond, NULL) == 0);
#define Cond_signal(cond) assert(pthread_cond_signal(cond) == 0);
#define Cond_wait(cond, mutex) assert(pthread_cond_wait(cond, mutex) == 0);

#ifdef __linux__
#define Sem_init(sem, value) assert(sem_init(sem, 0, value) == 0);
#define Sem_wait(sem) assert(sem_wait(sem) == 0);
#define Sem_post(sem) assert(sem_post(sem) == 0);
#endif // __linux__

#ifdef __APPLE__
typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
} sem_t;

void Sem_init(sem_t *sem, int value)
{
    sem->count = value;
    Pthread_mutex_init(&sem->mutex, NULL);
    Pthread_cond_init(&sem->cond, NULL);
}

void Sem_wait(sem_t *sem)
{
    Pthread_mutex_lock(&sem->mutex);
    while (sem->count <= 0)
    {
        Pthread_cond_wait(&sem->cond, &sem->mutex);
    }
    sem->count--;
    Pthread_mutex_unlock(&sem->mutex);
}

void Sem_post(sem_t *sem)
{
    Pthread_mutex_lock(&sem->mutex);
    sem->count++;
    Pthread_cond_signal(&sem->cond);
    Pthread_mutex_unlock(&sem->mutex);
}
#endif // __APPLE__

#endif // __common_threads_h__
