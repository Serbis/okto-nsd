//
// Created by serbis on 21.07.18.
//

#ifndef MNVP_DRIVER_MAP2_H
#define MNVP_DRIVER_MAP2_H

#include <stdint.h>
#include "colnode.h"
#include "list.h"

typedef struct MapItem {
    char *key;
    void *value;
} MapItem;

typedef struct Map {
    List *inner;
} Map;

typedef struct MapIterator {
    Map *map;
    Node* nextNode;
    int32_t lastRet;
} MapIterator;

void* MAP_get(char* key, Map *map);
void MAP_add(char* key, void* value, Map *map);
void* MAP_remove(char* key, Map *map);
bool MAP_contain(char *key, Map *map);
Map* MAP_del(Map *map);
Map* MAP_new();

MapIterator* MAP_ITERATOR_new(Map *map);
bool MAP_ITERATOR_hasNext(MapIterator *iterator);
void* MAP_ITERATOR_next(MapIterator *iterator);
void MAP_ITERATOR_remove(MapIterator *iterator);

#endif //MNVP_DRIVER_MAP2_H
