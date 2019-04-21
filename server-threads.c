//
// Created by kalesh on 4/16/19.
//

#include "server-threads.h"
#include "connect.h"
#include <stdlib.h>

char msg_buffer[MAX_SIZE];                                  // Message buffer
struct client_conn client_conns[MAX_CLIENT_CONNS];          // Client connections


pthread_mutex_t next_mutex;
pthread_cond_t next_cond;       // Condition variable to signal next msg available.

void *send_msg(void *args) {
    struct client_conn *a = (struct client_conn *) args;
    while (1) {
        pthread_mutex_lock(&(a->alive_mutex));        // LOCK
        if (a->alive == 1) {
            pthread_mutex_unlock(&(a->alive_mutex));  // UNLOCK
            acquire_shared();
            // Only send msg if client didn't disconnect.
            if (strlen(msg_buffer) > 0) {
                send(a->sockfd, (void *) a->msg_buffer, strlen(msg_buffer) + 1, 0);
                printf("Sent %s to FD %d\n", msg_buffer, a->sockfd);
            }
            release_shared();
            pthread_mutex_lock(&next_mutex);
            pthread_cond_wait(&next_cond, &next_mutex);
            pthread_mutex_unlock(&next_mutex);
        } else {
            break;
        }
    }
    pthread_mutex_unlock(&(a->alive_mutex));          // UNLOCK
    pthread_mutex_destroy(&(a->alive_mutex));         // Destroy the mutex to allow reinitialization for a future client
    // Free space allocated for message and usernaems
    free(a->msg.msg);
    free(a->msg.receiver);
    free(a->msg.sender);
    Close(a->sockfd);                                 // Close the connection socket
    printf("Closed FD %d\n", a->sockfd);
    a->cleanedup = 1;
    return NULL;
}

void *recv_msg(void *args) {
    char tmp_buffer[MAX_SIZE];
    struct client_conn *a = (struct client_conn *) args;
    while (1) {
        // No need to lock as only this thread can change the value of
        // the alive variable.
        if (a->alive == 1) {                              // UNLOCK
            int bytes_recv = recv(a->sockfd, (void *) &tmp_buffer, a->size, 0);

            acquire_exclusive();

            if (bytes_recv > 0) {
                printf("Recieved: %s\n", tmp_buffer);
                memcpy((void *) a->msg_buffer, (const void *) tmp_buffer, bytes_recv);
                printf("Wrote received msg to shared buffer.\n");
            } else {
                pthread_mutex_lock(&(a->alive_mutex));    // LOCK
                a->alive = 0;                             // Client no longer alive (i.e disconnected)
                msg_buffer[0] = '\0';
                printf("Client with FD %d disconnected\n", a->sockfd);
                pthread_mutex_unlock(&(a->alive_mutex));  // UNLOCK
            }

            pthread_mutex_lock(&next_mutex);
            pthread_cond_broadcast(&next_cond);
            pthread_mutex_unlock(&next_mutex);

            release_exclusive();
        } else {
            break;
        }
    }
    return NULL;
}

void init_client_conns() {
    init_rw_lock();                                 // Initialize the reader writer lock
    pthread_mutex_init(&next_mutex, NULL);
    pthread_cond_init(&next_cond, NULL);
    for (int i = 0; i < MAX_CLIENT_CONNS; i++) {    // All connections are initially not live
        client_conns[i].alive = 0;
        client_conns[i].cleanedup = 1;
    }
}

int next_client_conn() {
    for (int i = 0; i < MAX_CLIENT_CONNS; i++) {
        if (client_conns[i].alive == 0
            && client_conns[i].cleanedup == 1) {
            return i;
        }
    }
    return -1;
}

int add_client_conn(int sockfd) {
    int next = next_client_conn();
    if (next >= 0 && next < MAX_CLIENT_CONNS) {
        client_conns[next].sockfd = sockfd;
        client_conns[next].alive = 1;
        client_conns[next].cleanedup = 0;
        pthread_mutex_init(&(client_conns[next].alive_mutex), NULL);    // Initialize alive mutex
        client_conns[next].msg_buffer = msg_buffer;
        client_conns[next].size = MAX_SIZE;
        client_conns[next].msg.msg = malloc(MAX_LINE_SIZE * sizeof(char));
        client_conns[next].msg.sender = malloc(MAX_USERNAME_SIZE * sizeof(char));
        client_conns[next].msg.receiver = malloc(MAX_USERNAME_SIZE * sizeof(char));
        pthread_create(&(client_conns[next].send_thread), NULL, send_msg, (void *) &client_conns[next]);
        pthread_create(&(client_conns[next].recv_thread), NULL, recv_msg, (void *) &client_conns[next]);
        printf("Added new client with FD %d\n", sockfd);
    }
    return next;
}
