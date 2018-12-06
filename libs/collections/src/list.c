#include <stdint.h>
#include <stdbool.h>
#include "../include/list.h"
#include "../../oscl/include/malloc.h"

void LIST_prepend(List *this, void *item) {

    Node *node = pmalloc(sizeof(Node));
    node->item = item;
    node->next = this->head;

    this->head = node;
    this->size = (uint16_t) (this->size + 1);
}

void* LIST_find(List *this, bool (*p)(void *)) {
    Node *see = this->head;

    while (see != NULL) {
        if (p(see->item)) {
            return see->item;
        } else {
            see = see->next;
        }
    }

    return NULL;

}

void* LIST_get(List *this, uint16_t index) {
    Node *n = this->head;
    for (uint16_t i = 1; i <= index; i++) {
        if (n->next != NULL)
            n = n->next;
        else
            return NULL;
    }

    return n->item;
}


void* LIST_remove(List *this, uint16_t index) {
    Node *prev = NULL;
    Node *n = this->head;
    uint16_t target = 0;
    while (target < index) {
        if (target == index - 1)
            prev = n;
        n = n->next;
        target++;
    }

    if (prev != NULL) {
        prev->next = n->next;
        //this->size = (uint16_t) (this->size - 1);
    } else {
        this->head = n->next;
    }

    this->size--;

    void *item = n->item;
    pfree(n);
    return item;
}

void* LIST_ITERATOR_next(ListIterator *this) {
    Node *next = this->nextNode;
    this->nextNode = next->next;
    this->lastRet++;

    return next->item;
}

void LIST_ITERATOR_remove(ListIterator *this) {
    this->list->remove(this->list, (uint16_t) this->lastRet);
    this->lastRet--;
}

bool LIST_ITERATOR_hasNext(ListIterator *this) {
    return this->nextNode != NULL;
}

ListIterator* LIST_iterator(List *this) {
    ListIterator *iterator = pmalloc(sizeof(ListIterator));
    iterator->list = this;
    iterator->nextNode = this->head;
    iterator->lastRet = -1;
    iterator->next = LIST_ITERATOR_next;
    iterator->remove = LIST_ITERATOR_remove;
    iterator->hasNext = LIST_ITERATOR_hasNext;

    return iterator;
}

//Внимание, перед удалением списка, он должен быть очищен от элементов через итератор
void del_List(List *list) {
    pfree(list);
}

List* new_List() {
    List* list = (List*) pmalloc(sizeof(List));
    list->find = LIST_find;
    list->prepend = LIST_prepend;
    list->head = NULL;
    list->size = 0;
    list->get =  LIST_get;
    list->remove = LIST_remove;
    list->iterator =  LIST_iterator;


    return list;
}