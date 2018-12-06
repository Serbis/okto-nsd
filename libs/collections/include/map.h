//
// Created by serbis on 20.07.18.
//

#ifndef MNVP_DRIVER_MAP_H
#define MNVP_DRIVER_MAP_H

typedef struct map;

struct map* mapNew();
void mapAdd(char* key, void* val, struct map* map);
void mapDynAdd(char* key, void* val, struct map* map);
void* mapGet(char* key, struct map* map);
void mapClose(struct map* map);

#endif //MNVP_DRIVER_MAP_H
