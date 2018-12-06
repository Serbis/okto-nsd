//
// Created by serbis on 01.07.18.
//

#ifndef ACTORS_LIST_H
#define ACTORS_LIST_H

#include "colnode.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct ListIterator {
    struct List *list;
    Node* nextNode;
    int32_t lastRet;
    bool (*hasNext)(struct ListIterator*);
    void* (*next)(struct ListIterator*);
    void (*remove)(struct ListIterator*);
} ListIterator;

typedef struct List {
    Node *head;
    uint16_t size;
    void* (*find)(struct List*, bool (*)(void*));
    void (*prepend)(struct List*, void*);
    void* (*get)(struct List*, uint16_t);
    void* (*remove)(struct List*, uint16_t);
    ListIterator* (*iterator)(struct List*);
} List;

void del_List(List *list);
List* new_List();

#endif //ACTORS_LIST_H
