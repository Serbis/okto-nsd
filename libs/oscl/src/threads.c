#include <stdint.h>
#include <malloc.h>
#include "../include/threads.h"
#include "../include/malloc.h"


thread_t NewThread(void (*run)(void *), void *args, uint16_t stackSize, char *name, uint64_t priority) {
    if (name != NULL)
        pfree(name);
    pthread_t thread;
    int createerror = pthread_create(&thread, NULL, (void*) run, args);
    if (!createerror) {
        //pthread_join(thread, NULL);
        return thread;
    } else {
        return NULL;
    }
}

mutex_t* NewMutex() {
    pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);

    return mutex;
}

void MutexLock(mutex_t *mutex) {
    pthread_mutex_lock(mutex);
}

int MutexTryLock(mutex_t *mutex) {
    pthread_mutex_trylock(mutex);
}

void MutexUnlock(mutex_t *mutex) {
    pthread_mutex_unlock(mutex);
}