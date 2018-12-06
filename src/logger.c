/**
 * Internal daemon logger. Function from this file used for write formatted log to the file and stdout
 */

#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <time.h>
#include <stdbool.h>
#include <stdarg.h>
#include "../inc/logger.h"

/** Log file descriptor */
int fd = -1;

/** Write log messages to the stdout */
bool STDOUT = true;

/** Log entry format. For example [TIME][LEVEL][SOURCE] -> MESSAGE */
const char *logf = "[%d][%s][%s] -> %s\n";

/** see func def */
void Logger_log(char *msg);

/** Initialize logger. Open try to open the log file and return result of this operation
 *
 * @param fp path to the log file
 **/
int Logger_init(const char* fp) {
    fd = open(fp, O_RDWR | O_CREAT, 0644);
    return fd;
}

/** Write info log */
void Logger_info(char *source, char *str, ...) {
    size_t len = strlen(str);
    size_t lineSize = len + 200;

    char *msg = (char*) malloc(lineSize);

    va_list args;
    va_start(args, str);
    vsprintf(msg, str, args);
    va_end(args);

    char *m = (char*) malloc(100 + strlen(msg));

    sprintf(m, logf, time(0), "INFO",source, msg);
    Logger_log(m);
}

/** Write fatal log */
void Logger_fatal(char *source, char *str, ...) {
    size_t len = strlen(str);
    size_t lineSize = len + 200;

    char *msg = (char*) malloc(lineSize);

    va_list args;
    va_start(args, str);
    vsprintf(msg, str, args);
    va_end(args);

    char *m = (char*) malloc(100 + strlen(msg)); //FIXME не вижу освобождения памяти

    sprintf(m, logf, time(0), "FATAL",source, msg);
    Logger_log(m);
}

/** Internal function that`s do all work */
void Logger_log(char *msg) {
    if (fd == NULL) {
        printf("Unable to write log to the file -> %s", msg);
        fflush(stdout);
    } else {
        lseek(fd, 0L, SEEK_END);
        size_t len = strlen(msg);
        uint8_t *bf = malloc(len);
        memcpy(bf, msg, len);
        write(fd, bf, len);
        if (STDOUT) {
            printf(msg);
            fflush(stdout);
        }
        free(bf);
    }
}