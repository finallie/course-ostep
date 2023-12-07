#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "common_threads.h"

//
// Your code goes in the structure and functions below
//

typedef struct __rwlock_t
{
    pthread_mutex_t lock;
    pthread_cond_t writelock;
    int readers;
    int waiting;
    bool writer;
} rwlock_t;

void rwlock_init(rwlock_t *rw)
{
    Pthread_mutex_init(&rw->lock, NULL);
    pthread_cond_init(&rw->writelock, NULL);
    rw->readers = 0;
    rw->waiting = 0;
    rw->writer = false;
}

void rwlock_acquire_readlock(rwlock_t *rw)
{
    Pthread_mutex_lock(&rw->lock);
    while (rw->waiting > 0 || rw->writer)
    {
        Pthread_cond_wait(&rw->writelock, &rw->lock);
    }
    rw->readers++;
    Pthread_mutex_unlock(&rw->lock);
}

void rwlock_release_readlock(rwlock_t *rw)
{
    Pthread_mutex_lock(&rw->lock);
    rw->readers--;
    if (rw->readers == 0)
    {
        pthread_cond_broadcast(&rw->writelock);
    }
    Pthread_mutex_unlock(&rw->lock);
}

void rwlock_acquire_writelock(rwlock_t *rw)
{
    Pthread_mutex_lock(&rw->lock);
    rw->waiting++;
    while (rw->readers > 0 || rw->writer)
    {
        Pthread_cond_wait(&rw->writelock, &rw->lock);
    }
    rw->waiting--;
    rw->writer = true;
    Pthread_mutex_unlock(&rw->lock);
}

void rwlock_release_writelock(rwlock_t *rw)
{
    Pthread_mutex_lock(&rw->lock);
    rw->writer = false;
    pthread_cond_broadcast(&rw->writelock);
    Pthread_mutex_unlock(&rw->lock);
}

//
// Don't change the code below (just use it!)
//

int loops;
int value = 0;

rwlock_t lock;

void *reader(void *arg)
{
    int i;
    for (i = 0; i < loops; i++)
    {
        rwlock_acquire_readlock(&lock);
        printf("read %d\n", value);
        rwlock_release_readlock(&lock);
    }
    return NULL;
}

void *writer(void *arg)
{
    int i;
    for (i = 0; i < loops; i++)
    {
        rwlock_acquire_writelock(&lock);
        value++;
        printf("write %d\n", value);
        rwlock_release_writelock(&lock);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    assert(argc == 4);
    int num_readers = atoi(argv[1]);
    int num_writers = atoi(argv[2]);
    loops = atoi(argv[3]);

    pthread_t pr[num_readers], pw[num_writers];

    rwlock_init(&lock);

    printf("begin\n");

    int i;
    for (i = 0; i < num_readers; i++)
        Pthread_create(&pr[i], NULL, reader, NULL);
    for (i = 0; i < num_writers; i++)
        Pthread_create(&pw[i], NULL, writer, NULL);

    for (i = 0; i < num_readers; i++)
        Pthread_join(pr[i], NULL);
    for (i = 0; i < num_writers; i++)
        Pthread_join(pw[i], NULL);

    printf("end: value %d\n", value);

    return 0;
}
