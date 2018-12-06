#include "../include/data.h"

char* itoa2(int num) {
    char *buf = pmalloc(50);
    sprintf(buf, "%d", num);
    uint8_t len = (uint8_t) (strlen(buf) + 1);
    char *split = pmalloc(len);
    memcpy(split, buf, len);
    pfree(buf);

    return split;
}

char* strcpy2(char* str) {
    size_t strsize = strlen(str) + 1;
    char* dup_str = pmalloc(strsize);
    memcpy(dup_str, str, strsize);

    return dup_str;
}