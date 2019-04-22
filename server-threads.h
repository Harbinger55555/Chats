//
// Created by kalesh on 4/16/19.
//

#ifndef CHATS_SERVER_THREADS_H
#define CHATS_SERVER_THREADS_H

#include "lock.h"
#include "message.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#define MAX_CLIENT_CONNS 1000

struct client_conn {
    int sockfd;
    volatile int alive;
    int cleanedup;
    pthread_t send_thread;
    pthread_t recv_thread;
    pthread_mutex_t alive_mutex;    // Mutex to enforce atomicity for alive check
    struct message msg;
};

void *send_msg(void *args);

void *recv_msg(void *args);

void init_client_conns();

int next_client_conn();

int add_client_conn(int sockfd);

#endif //CHATS_SERVER_THREADS_H
