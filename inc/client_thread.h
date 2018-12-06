//
// Created by serbis on 22.10.18.
//

#ifndef NSD_CLIENT_THREAD_H
#define NSD_CLIENT_THREAD_H

#include <stdbool.h>

bool clientThread_alive;

void ClientThread_run(void *args);

#endif //NSD_CLIENT_THREAD_H
