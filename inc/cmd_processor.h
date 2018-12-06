//
// Created by serbis on 22.10.18.
//

#ifndef NSD_CMD_PROCESSOR_H
#define NSD_CMD_PROCESSOR_H

#include <stdbool.h>
#include "../libs/collections/include/lbq.h"

/** Thread args */
typedef struct CmdProcessor_Args {
    LinkedBlockingQueue *cmdQueue;
    int sockfd;
    bool alive;
} CmdProcessor_Args;

char* CmdProcessor_createResponse(const char *type, uint32_t msgId, const char *content);
void CmdProcessor_run(void *args);

#endif //NSD_CMD_PROCESSOR_H
