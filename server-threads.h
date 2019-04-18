//
// Created by kalesh on 4/16/19.
//

#ifndef CHATS_SERVER_THREADS_H
#define CHATS_SERVER_THREADS_H

#include "lock.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#define MAX_SIZE 1000
#define MAX_CLIENT_CONNS 1000

struct client_conn {
    int sockfd;
    volatile int alive;
    pthread_t send_thread;
    pthread_t recv_thread;
};

struct thread_args {
    char *msg_buffer;
    size_t size;
    struct client_conn *conn;
};

extern char msg_buffer[MAX_SIZE];                           // Message buffer
extern struct client_conn client_conns[MAX_CLIENT_CONNS];   // Client connections
extern pthread_mutex_t alive_mutex;                         // Mutex to enforce atomicity for alive check

void *send_msg(void *args);

void *recv_msg(void *args);

void init_client_conns();

int next_client_conn();

int add_client_conn(int sockfd);

#endif //CHATS_SERVER_THREADS_H
