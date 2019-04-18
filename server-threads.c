//
// Created by kalesh on 4/16/19.
//

#include "server-threads.h"
#include "connect.h"

char msg_buffer[MAX_SIZE];                                  // Message buffer
struct client_conn client_conns[MAX_CLIENT_CONNS];          // Client connections

void *send_msg(void *args) {
    struct thread_args *a = (struct thread_args *) args;
    while (1) {
        pthread_mutex_lock(&(a->conn->alive_mutex));        // LOCK
        if (a->conn->alive == 1) {
            pthread_mutex_unlock(&(a->conn->alive_mutex));  // UNLOCK
            acquire_shared();
            send(a->conn->sockfd, (void *) a->msg_buffer, a->size, 0);
            release_shared();
        } else {
            break;
        }
    }
    pthread_mutex_unlock(&(a->conn->alive_mutex));          // UNLOCK
    pthread_mutex_destroy(&(a->conn->alive_mutex));         // Destroy the mutex to allow reinitialization for a future client
    Close(a->conn->sockfd);                                 // Close the connection socket
    return NULL;
}

void *recv_msg(void *args) {
    char tmp_buffer[MAX_SIZE];
    struct thread_args *a = (struct thread_args *) args;
    while (1) {
        pthread_mutex_lock(&(a->conn->alive_mutex));            // LOCK
        if (a->conn->alive == 1) {                              // UNLOCK
            pthread_mutex_unlock(&(a->conn->alive_mutex));
            if (recv(a->conn->sockfd, (void *) &tmp_buffer, a->size, 0) == 0) {
                pthread_mutex_lock(&(a->conn->alive_mutex));    // LOCK
                a->conn->alive = 0;                             // Client no longer alive (i.e disconnected)
                pthread_mutex_unlock(&(a->conn->alive_mutex));  // UNLOCK
            } else {
                acquire_exclusive();
                printf("Recieved: %s\n", tmp_buffer);
                memcpy((void *) a->msg_buffer, (const void *) &tmp_buffer, MAX_SIZE);
                release_exclusive();
            }
        } else {
            break;
        }
    }
    pthread_mutex_unlock(&(a->conn->alive_mutex));              // UNLOCK
    return NULL;
}

void init_client_conns() {
    init_rw_lock();                                 // Initialize the reader writer lock
    for (int i = 0; i < MAX_CLIENT_CONNS; i++) {    // All connections are initially not live
        client_conns[i].alive = 0;
    }
}

int next_client_conn() {
    for (int i = 0; i < MAX_CLIENT_CONNS; i++) {
        if (client_conns[i].alive == 0) {
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
        pthread_mutex_init(&(client_conns[next].alive_mutex), NULL);    // Initialize alive mutex
        struct thread_args args = {msg_buffer, MAX_SIZE, &(client_conns[next])};
        pthread_create(&(client_conns[next].send_thread), NULL, send_msg, (void *) &args);
        pthread_create(&(client_conns[next].recv_thread), NULL, recv_msg, (void *) &args);
    }
    return next;
}
