#include <malloc.h>
#include "../include/malloc.h"

void *pmalloc (size_t __size) {
    return malloc(__size);
}

void pfree(void *__ptr) {
    free(__ptr);
}

