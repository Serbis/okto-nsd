//
// Created by serbis on 01.07.18.
//

#ifndef ACTORS_THREADS_H
#define ACTORS_THREADS_H

#include <pthread.h>
#include <stdint.h>

typedef pthread_t thread_t;
typedef pthread_mutex_t mutex_t;

thread_t NewThread(void (*run)(void *), void *args, uint16_t stackSize, char *name, uint64_t priority);
mutex_t* NewMutex();
void MutexLock(mutex_t *mutex);
int MutexTryLock(mutex_t *mutex);
void MutexUnlock(mutex_t *mutex);

#endif //ACTORS_THREADS_H
