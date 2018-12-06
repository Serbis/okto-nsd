#include <unistd.h>
#include "../include/time.h"

//Return current system time in millis
uint64_t SystemTime() {
    return (uint64_t) time(NULL) * 1000;
}

//Delay current thread for some millis
void DelayMillis(uint64_t millis) {
    usleep((__useconds_t) (millis * 1000));
}