#include <memory.h>
#include "../include/map2.h"
#include "../../oscl/include/malloc.h"
#include "../../oscl/include/data.h"

//TODO сделать данную коллекцию потокобезопасной
void* MAP_get(char* key, Map *map) {
    ListIterator *iterator = map->inner->iterator(map->inner);
    while (iterator->hasNext(iterator)) {
        MapItem *item = iterator->next(iterator);
        if (strcmp(item->key, key) == 0) {
            pfree(iterator);
            return item->value;
        }
    }

    pfree(iterator);

    return NULL;
}

void MAP_add(char* key, void* value, Map *map) {
    MapItem *item = pmalloc(sizeof(MapItem));
    item->key = strcpy2(key);
    item->value = value;

    map->inner->prepend(map->inner, item);
}

void* MAP_remove(char* key, Map *map) {
    ListIterator *iterator = map->inner->iterator(map->inner);
    uint16_t index = 0;
    while (iterator->hasNext(iterator)) {
        MapItem *item = iterator->next(iterator);
        if (strcmp(item->key, key) == 0) {
            pfree(iterator);
            map->inner->remove(map->inner, index);
            void *value = item->value;
            pfree(item->key);
            pfree(item);

            return value;
        } else {
            index++;
        }
    }

    pfree(iterator);

    return NULL;
}

//TODO Удаление коллекции

bool MAP_contain(char *key, Map *map) {
    ListIterator *iterator = map->inner->iterator(map->inner);

    while (iterator->hasNext(iterator)) {
        MapItem *item = iterator->next(iterator);
        if (strcmp(item->key, key) == 0) {
            pfree(iterator);
            return true;
        }
    }

    pfree(iterator);

    return false;
}

//Внимание, перед вызовом данной функции, нужно очистить inner от элементов
Map* MAP_del(Map *map) {
    del_List(map->inner);
    pfree(map);
}

Map* MAP_new() {
    Map* map = (Map*) pmalloc(sizeof(Map));
    map->inner = new_List();

    return map;
}

MapIterator* MAP_ITERATOR_new(Map *map) {
    MapIterator *iterator = pmalloc(sizeof(MapIterator));
    iterator->map = map;
    iterator->nextNode = map->inner->head;
    iterator->lastRet = -1;
}

bool MAP_ITERATOR_hasNext(MapIterator *iterator) {
    return iterator->nextNode != NULL;
}

void* MAP_ITERATOR_next(MapIterator *iterator) {
    Node *next = iterator->nextNode;
    iterator->nextNode = next->next;
    iterator->lastRet++;

    return ((MapItem*)next->item)->value;
}

void MAP_ITERATOR_remove(MapIterator *iterator) {
    MapItem *item = iterator->map->inner->remove(iterator->map->inner, (uint16_t) iterator->lastRet);
    pfree(item->key);
    pfree(item);
    iterator->lastRet--;
}