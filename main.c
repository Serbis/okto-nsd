/** This program is a system daemon of Okto node. The task of this daemon is to change the parameters of the system
 *  requiring superuser privileges. Since the launch of the main part of the system from under privileged users is
 *  not possible for security reasons, a specialized daemon was developed that operates at the level of a limited list
 *  of applied commands. At the top level, this program is a classic unix fork daemon. At the bottom, for communication
 *  with other processes, it represents the unix domain socket nsd.socket. Communication with the program takes place
 *  in the request-response mode using the OTPP (Okto Text Packet Protocol) protocol. For obvious reasons, this program
 *  should run as root user linux.
 *
 *  Hot to compile program for target architecture:
 *      1. Upload tar with project to target device
 *      2. Put it to path that`s full clone project path in your workstation
 *      3. Change min cmake version to version of cmake installed on target device
 *      4. Run command /usr/bin/cmake --build /path/to/project//cmake-build-release --target nsd -- -j 1
 *
 *  */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>
#include <signal.h>
#include "inc/client_thread.h"
#include "inc/logger.h"
#include "libs/oscl/include/data.h"

// ================================ GLOBAL VARIABLES ====================================

char *pid_path = "/var/run/nsd.pid";

/** Start domain server. This server listen for incoming bind request. After accept a connection, it create
 *  new client thread with client socket id, and try to accept a new connections */
void startServer() {
    int server_sockfd, client_sockfd;
    int server_len, client_len;
    struct sockaddr_un server_address;
    struct sockaddr_un client_address;
    char *socket_path = "/tmp/nsd.socket";

    unlink(socket_path);
    server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, socket_path);
    server_len = sizeof(server_address);
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

    listen(server_sockfd, 5);

    Logger_info("Server", "Server listen '%s'", socket_path);
    while(1) {
        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address, &client_len);

        if (client_sockfd == -1) {
            Logger_fatal("Server", "Unable to open socket '%s'", socket_path);
            exit(-1);
        }
        pthread_t thread;
        int *sockfd = (int*) malloc(4);
        *sockfd = client_sockfd;
        pthread_create(&thread, NULL, (void *) ClientThread_run, sockfd);
    }
}

/** Daemon signal handler. This handler handle only one signal SIGUSR1. This signal sent by daemon launcher from
 *  stop logic branch. After receive this signal daemon will terminates */
void signalHandler(int sig) {
    if (sig == SIGUSR1) {
        exit(0);
    }
}

/** Start new daemon instance. It fork the program process and write it pid to the nsd.pid file. Child process
 *  run new domain socket server. */
int start() {
    //check pid file existing
    if (access(pid_path, 0) == 0)  {
        Logger_fatal("DaemonRunner/start", "Daemon already started");

        return -1;
    }

    int pid = fork();

    if (pid == -1) { // если не удалось запустить потомка
        Logger_fatal("DaemonRunner", "Start Daemon failed (%s)", strerror(errno));

        return -1;
    } else if (!pid) { // если это потомок
        umask(0);
        setsid();

        signal(SIGUSR1, signalHandler);
        startServer();

        return 0;
    } else { // если это родитель
        int pidf = open(pid_path, O_RDWR | O_CREAT | O_TRUNC, 0644);

        if (pidf == -1) {
            Logger_fatal("DaemonRunner/start", "Unable to create pid file");

            return -1;
        }

        char *pstr = itoa2(pid);
        if (write(pidf, pstr, strlen(pstr)) <= 0) {
            Logger_fatal("DaemonRunner/start", "Unable to write pid file");

            return -1;
        }
        free(pstr);

        Logger_info("DaemonRunner/start", "Daemon has been successfully started with pid '%d'", pid);

        return 0;
    }
}

/** Stop current daemon instance. It is send SIGUSR1 to the daemon process with pid from nsd.pid file and remove thi
 * file */
int stop() {
    //check pid file existing
    if (access(pid_path, 0) != 0)  {
        Logger_fatal("DaemonRunner/stop", "Daemon does not started");

        return -1;
    } else {
        int pidf = open(pid_path, O_RDWR, 0644);

        if (pidf == -1) {
            Logger_fatal("DaemonRunner/stop", "Unable to open pid file");

            return -1;
        }

        lseek(pidf, 0L, SEEK_END);
        size_t fSize = (size_t) lseek(pidf, 0, SEEK_CUR);
        lseek(pidf, 0L, SEEK_SET);

        char *pids = malloc(fSize);
        read(pidf, pids, fSize);
        int pid = atoi(pids);
        free(pids);

        Logger_info("DaemonRunner/stop", "Daemon with pid '%d' has been requested to stopped", pid);

        kill(pid, SIGUSR1); //Send stop signal to the daemon process

        remove(pid_path);
    }
}

/** Stop and Start daemon */
int restart() {
    int r1 = stop();
    int r2 = start();

    if (r1 < 0 || r2 < 0 ) {
        Logger_fatal("DaemonRunner/restart", "Unable to restart daemon");

        return -1;
    }

    return 0;
}

/** Return status of the current daemon instance */
int status() {
    if (access(pid_path, 0) != 0)  {
        Logger_fatal("DaemonRunner/status", "Daemon does not started");

        return 0;
    } else {
        int pidf = open(pid_path, O_RDWR, 0644);

        if (pidf == -1) {
            Logger_fatal("DaemonRunner/status", "Unable to open pid file");

            return -1;
        }

        lseek(pidf, 0L, SEEK_END);
        size_t fSize = (size_t) lseek(pidf, 0, SEEK_CUR);
        lseek(pidf, 0L, SEEK_SET);

        char *pids = malloc(fSize);
        read(pidf, pids, fSize);
        int pid = atoi(pids);
        free(pids);

        Logger_info("DaemonRunner/status", "Daemon run with pid '%d'", pid);
    }
}

/** Parse input arg and run specified subprogram */
int daemonRun(int argc, char* argv[]) {
    if (argc < 2) {
        Logger_fatal("DaemonRunner", "No action specified");

        return -1;
    }

    if (strcmp(argv[1], "start") == 0) {
        return start();
    } else if(strcmp(argv[1], "stop") == 0) {
        return stop();
    } else if(strcmp(argv[1], "restart") == 0) {
        return restart();
    } else if(strcmp(argv[1], "status") == 0) {
        return status();
    } else {
        Logger_fatal("DaemonRunner", "Unknown action '%s'", argv[1]);
    }


}

/** This function used to start program in the developing mode. In this mode, fork does not take place, and execution
 *  immediately begins from startServer function */
int devRun(int argc, char* argv[]) {
    startServer();

    return 0;
}

int main(int argc, char* argv[]) {
    if (!Logger_init("/var/log/nsd.log")) {
        printf("Unable to initialize logger. Program will be terminated!");
        fflush(stdout);
        exit(1);
    }

    return daemonRun(argc, argv);
}