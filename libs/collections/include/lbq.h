//
// Created by serbis on 01.07.18.
//

#ifndef ACTORS_LBQ_H
#define ACTORS_LBQ_H

#include <stdint.h>
#include <malloc.h>
#include "colnode.h"
#include "../../oscl/include/threads.h"

typedef struct LinkedBlockingQueue {
    uint16_t capacity;
    uint16_t count;
    Node *head;
    Node *last;
    mutex_t *mutex;

    void (*enqueue)(void*, void*);
    void* (*dequeue)(void*);
    uint16_t (*size)(void*);
} LinkedBlockingQueue;

void del_LQB(LinkedBlockingQueue *queue);
LinkedBlockingQueue* new_LQB(uint16_t capacity);

#endif //ACTORS_LBQ_H
