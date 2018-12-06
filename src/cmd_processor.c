/** Main command processing thread. It is a paired with client thread. Clint thread parses the packets and put
 * it's to the queue. And this thread dequeue packet, parse it and execute some command from it. */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../inc/cmd_processor.h"
#include "../inc/logger.h"
#include "../libs/collections/include/lbq.h"
#include "../libs/collections/include/list.h"
#include "../libs/oscl/include/data.h"
#include "../libs/oscl/include/time.h"

/** Internal function. Create packaged response from input data */
char* CmdProcessor_createResponse(const char *type, uint32_t msgId, const char *content) {
    char *msgIdStr = itoa2(msgId);
    size_t size = strlen(type) + 2 + strlen(msgIdStr) + 2 + strlen(content);
    char *buf = pmalloc(size);
    sprintf(buf, "%s\t%s\t%s\r", type, msgIdStr, content);

    pfree(msgIdStr);

    return buf;
}

void CmdProcessor_notEnoughArgs(uint32_t packetId, int sockfd) {
    char *resp = CmdProcessor_createResponse("e", packetId, "Not enough arguments");
    write(sockfd, resp, strlen(resp));
    pfree(resp);
}

//============================================== COMMANDS =================================================

/** Get daemon version */
void CmdProcessor_cmd_version(uint32_t packetId, int sockfd) {
    Logger_info("CmdProcessor", "Received 'version' command");
    char *resp = CmdProcessor_createResponse("r", packetId, "0.0.1");
    write(sockfd, resp, strlen(resp));
    pfree(resp);
}

/** Test Function. Echoing first argument */
void CmdProcessor_cmd_echo(uint32_t packetId, int sockfd, char *str) {
    Logger_info("CmdProcessor", "Received 'echo' command with arg '%s'", str);
    char *resp = CmdProcessor_createResponse("r", packetId, str);
    write(sockfd, resp, strlen(resp));
    pfree(resp);
}

/** Test Function. Stop thread for ms specified in first arg */
void CmdProcessor_cmd_tmt(uint32_t packetId, int sockfd, long delay) {
    Logger_info("CmdProcessor", "Received 'tmt' command with arg '%d'", delay);
    DelayMillis((uint64_t) delay);
    char *resp = CmdProcessor_createResponse("r", packetId, "ok");
    write(sockfd, resp, strlen(resp));
    pfree(resp);
}

/** Test Function. Return first arg as content of error response packet */
void CmdProcessor_cmd_err(uint32_t packetId, int sockfd, char *str) {
    Logger_info("CmdProcessor", "Received 'err' command");
    char *resp = CmdProcessor_createResponse("e", packetId, str);
    write(sockfd, resp, strlen(resp));
    pfree(resp);
}

/** Test Function. Return broken packet by type from first arg */
void CmdProcessor_cmd_re(uint32_t packetId, int sockfd, char *type) {
    Logger_info("CmdProcessor", "Received 're' with type '%s'", type);
    char *resp;
    bool mf = false;
    if (strcmp("0", type) == 0) {
        resp = "_\t1\terror\r"; //Incorrect qualifier
    } else if (strcmp("1", type) == 0) {
        resp = "r\terror\r"; //Incorrect struct
    } else if (strcmp("2", type) == 0) {
        resp = "r\ta\terror\r"; //Broken msgid block
    } else if (strcmp("3", type) == 0) {
        resp = "r\t1234\terror\r";  //Arbitrary msgid
    } else {
        mf = true;
        resp = CmdProcessor_createResponse("e", packetId, "Unknown type");
    }
    write(sockfd, resp, strlen(resp));
    if (mf)
        pfree(resp);
}


//=========================================== THREAD FUNCTION =============================================

/** Main thread function. It parses incoming packets and execute some commands based on it`s content */
void CmdProcessor_run(void *args) {
    Logger_info("CmdProcessor", "Command processor thread was started");

    CmdProcessor_Args *params = (CmdProcessor_Args*) args;

    LinkedBlockingQueue *cmdQueue = params->cmdQueue;
    while(params->alive) {
        if (cmdQueue->size(cmdQueue) > 0) {
            char *cmd = cmdQueue->dequeue(cmdQueue);
            char *spl = strtok(cmd, "\t");
            List *fields = new_List();

            while (spl != NULL)  {
                fields->prepend(fields, strcpy2(spl));
                spl = strtok (NULL, "\t");
            }

            //----------------------------------------------------------------------------

            if (fields->size < 3) {
                pfree(cmd);
                pfree(spl);
                ListIterator *iterator = fields->iterator(fields);
                while(iterator->hasNext(iterator)) {
                    void *v = iterator->next(iterator);
                    iterator->remove(iterator);
                    pfree(v);
                }
                pfree(iterator);
                pfree(fields);
                continue;
            }

            char *packetType = fields->get(fields, 2); // Тип пакета
            uint32_t packetId = (uint32_t) strtol(fields->get(fields, 1), NULL, 10); // Идентификатор пакета
            List *packetElements = new_List(); // Элементы

            if (packetId == 0) {
                pfree(cmd);
                pfree(spl);
                ListIterator *iterator = fields->iterator(fields);
                while(iterator->hasNext(iterator)) {
                    void *v = iterator->next(iterator);
                    iterator->remove(iterator);
                    pfree(v);
                }
                pfree(iterator);
                pfree(fields);

                ListIterator *iterator2 = packetElements->iterator(packetElements);
                while(iterator2->hasNext(iterator2)) {
                    void *v = iterator2->next(iterator2);
                    iterator2->remove(iterator2);
                    pfree(v);
                }
                pfree(iterator2);
                pfree(packetElements);
                continue;
            }

            char *inCmdSpl = strtok(fields->get(fields, 0), " ");
            while (inCmdSpl != NULL)  {
                if (strchr(inCmdSpl, '\r') != NULL) {
                    size_t strLen = strlen(inCmdSpl);
                    char *nInCmdSpl = pmalloc(strLen);
                    nInCmdSpl[strLen - 1] = 0;

                    strncpy(nInCmdSpl, inCmdSpl, strLen - 1);
                    if (strlen(inCmdSpl) != 0)
                        packetElements->prepend(packetElements, strcpy2(nInCmdSpl));
                    pfree(nInCmdSpl);

                } else {
                    packetElements->prepend(packetElements, strcpy2(inCmdSpl));
                }
                inCmdSpl = strtok (NULL, " ");
            }

            if (packetElements->size == 0) {
                pfree(cmd);
                pfree(spl);
                ListIterator *iterator = fields->iterator(fields);
                while(iterator->hasNext(iterator)) {
                    void *v = iterator->next(iterator);
                    iterator->remove(iterator);
                    pfree(v);
                }
                pfree(iterator);
                pfree(fields);

                ListIterator *iterator2 = packetElements->iterator(packetElements);
                while(iterator2->hasNext(iterator2)) {
                    void *v = iterator2->next(iterator2);
                    iterator2->remove(iterator2);
                    pfree(v);
                }
                pfree(iterator2);
                pfree(packetElements);
                continue;
            }

            //----------------------------------------------

            uint16_t cmdSize = packetElements->size;

            char *cmdName = packetElements->get(packetElements, (uint16_t) (cmdSize - 1));

            /*if (strcmp(cmdName, "readAdc") == 0) {
                char *inNum = packetElements->get(packetElements, (uint16_t) (cmdSize - 2));
                if (inNum != NULL) {
                    long inNumInt = strtol(inNum, NULL, 10);
                    if (inNumInt != 0) {
                        CmdProcessor_cmd_adcRead(packetId, (uint8_t) inNumInt);
                    } else {
                        char *resp = CmdProcessor_createResponse("e", packetId, "First arg must be a number");
                        Hardware_writeToUart(resp);
                        pfree(resp);
                    }
                } else {
                    char *resp = CmdProcessor_createResponse("e", packetId, "Not enough arguments");
                    Hardware_writeToUart(resp);
                    pfree(resp);
                }
            }*/

            if (strcmp(cmdName, "version") == 0) {
                CmdProcessor_cmd_version(packetId, params->sockfd);
            } else if (strcmp(cmdName, "t_echo") == 0) {
                char *str = packetElements->get(packetElements, (uint16_t) (cmdSize - 2));
                if (str != NULL)
                    CmdProcessor_cmd_echo(packetId, params->sockfd, str);
                else
                    CmdProcessor_notEnoughArgs(packetId, params->sockfd);
            } else if (strcmp(cmdName, "t_tmt") == 0) {
                char *delay = packetElements->get(packetElements, (uint16_t) (cmdSize - 2));
                if (delay != NULL) {
                    long delayInt = strtol(delay, NULL, 10);
                    if (delayInt != 0) {
                        CmdProcessor_cmd_tmt(packetId, params->sockfd, delayInt);
                    } else {
                        char *resp = CmdProcessor_createResponse("e", packetId, "First arg must be a number");
                        write(params->sockfd, resp, strlen(resp));
                        pfree(resp);
                    }
                } else {
                    CmdProcessor_notEnoughArgs(packetId, params->sockfd);
                }
            } else if (strcmp(cmdName, "t_err") == 0) {
                char *str = packetElements->get(packetElements, (uint16_t) (cmdSize - 2));
                if (str != NULL)
                    CmdProcessor_cmd_err(packetId, params->sockfd, str);
                else
                    CmdProcessor_notEnoughArgs(packetId, params->sockfd);
            } else if (strcmp(cmdName, "t_re") == 0) {
                char *type = packetElements->get(packetElements, (uint16_t) (cmdSize - 2));
                if (type != NULL)
                    CmdProcessor_cmd_re(packetId, params->sockfd, type);
                else
                    CmdProcessor_notEnoughArgs(packetId, params->sockfd);
            } else {
                Logger_info("CmdProcessor", "Received unknown command '%s'", cmdName);
                char *resp = CmdProcessor_createResponse("e", packetId, "Unknown command");
                write(params->sockfd, resp, strlen(resp));
                pfree(resp);
            }

            pfree(cmd);
            if (cmd != NULL)
                pfree(spl);
            if (inCmdSpl != NULL)
                pfree(inCmdSpl);
            ListIterator *iterator = fields->iterator(fields);
            while(iterator->hasNext(iterator)) {
                void *v = iterator->next(iterator);
                iterator->remove(iterator);
                pfree(v);
            }
            pfree(iterator);
            pfree(fields);

            ListIterator *iterator2 = packetElements->iterator(packetElements);
            while(iterator2->hasNext(iterator2)) {
                void *v = iterator2->next(iterator2);
                iterator2->remove(iterator2);
                pfree(v);
            }
            pfree(iterator2);
            pfree(packetElements);
        } else {
            DelayMillis(100);
        }
    }

    free(args);
    while(cmdQueue->size(cmdQueue) > 0) {
        char *v = cmdQueue->dequeue(cmdQueue);
        free(v);
    }
    del_LQB(cmdQueue);

    Logger_info("CmdProcessor", "Command processor thread was stopped");
}