/** Thread that's read data from the socket. Its task is to build packets from the incoming byte stream and place them
 *  in a queue of received packets. */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../inc/client_thread.h"
#include "../libs/collections/include/rings.h"
#include "../libs/collections/include/lbq.h"
#include "../inc/logger.h"
#include "../inc/cmd_processor.h"
#include "../libs/oscl/include/time.h"


void ClientThread_run(void *args) {
    bool clientThread_alive = true;
    int sockfd = *(int*) args;
    Logger_info("ClientThread", "Client thread for sockdf '%d' was started", sockfd);
    LinkedBlockingQueue *cmdQueue = new_LQB(50); //Free in cmd_processor
    RingBufferDef *inBuf  = RINGS_createRingBuffer(500, RINGS_OVERFLOW_SHIFT, true);
    CmdProcessor_Args *cpa = malloc(sizeof(CmdProcessor_Args));  //Free in cmd_processor
    cpa->cmdQueue = cmdQueue;
    cpa->sockfd = sockfd;
    cpa->alive = true;
    thread_t cmdp_t = NewThread(CmdProcessor_run, cpa, 0, NULL, 0);


    while(clientThread_alive) {
        char ch;
        ssize_t r = read(sockfd, &ch, 1);
        if (r > 0) { // if some char was received
            if (ch == '\r') {
                RINGS_write((uint8_t) ch, inBuf);
                uint16_t len = RINGS_dataLenght(inBuf);
                char *str = malloc(len + 1);
                str[len] = 0;
                RINGS_extractData(inBuf->writer - len, len, (uint8_t *) str, inBuf);
                RINGS_dataClear(inBuf);
                cmdQueue->enqueue(cmdQueue, str);
            } else {
                RINGS_write((uint8_t) ch, inBuf);
            }
        } else {
            Logger_info("ClientThread", "Socket is closed");
            clientThread_alive = false;
        }
    }

    cpa->alive = false;
    RINGS_Free(inBuf);
    close(sockfd);
    Logger_info("ClientThread", "Client thread for sockdf '%d' was stopped", sockfd);
}