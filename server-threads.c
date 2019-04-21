//
// Created by kalesh on 4/16/19.
//

#include "server-threads.h"
#include "connect.h"

char msg_buffer[MAX_SIZE] = {0};                                  // Message buffer
struct client_conn client_conns[MAX_CLIENT_CONNS];          // Client connections


void *send_msg(void *args) {
    struct client_conn *a = (struct client_conn *) args;
    while (1) {
        pthread_mutex_lock(&(a->alive_mutex));        // LOCK
        printf("Send FD %d obtained alive mutex\n", a->sockfd);
        if (a->alive == 1) {
            printf("Send FD %d alive = %d\n", a->sockfd, a->alive);
            pthread_mutex_unlock(&(a->alive_mutex));  // UNLOCK
            printf("Send FD %d released alive mutex\n", a->sockfd);
            pthread_mutex_lock(&(a->a_sent_mutex));
            printf("Send FD %d obtained already sent mutex\n", a->sockfd);
            acquire_shared(a->already_sent);
            printf("Send FD %d obtained acquire shared\n", a->sockfd);
            // Only send msg if client didn't disconnect.
            if (strlen(msg_buffer) > 0) {
                send(a->sockfd, (void *) a->msg_buffer, strlen(msg_buffer) + 1, 0);
                printf("Sent %s to FD %d\n", msg_buffer, a->sockfd);
                a->already_sent = 1;
            }
            pthread_mutex_unlock(&(a->a_sent_mutex));
            printf("Send FD %d released already sent mutex\n", a->sockfd);
            release_shared();
            printf("Send FD %d released shared mutex\n", a->sockfd);
        } else {
            printf("Send FD %d alive = %d\n", a->sockfd, a->alive);
            break;
        }
    }
    pthread_mutex_unlock(&(a->alive_mutex));          // UNLOCK
    printf("Send FD %d released alive mutex\n", a->sockfd);

    pthread_mutex_destroy(&(a->alive_mutex));         // Destroy the mutex to allow reinitialization for a future client
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

            pthread_mutex_lock(&(a->a_sent_mutex));
            printf("Recv FD %d acquired already sent mutex\n", a->sockfd);
            acquire_exclusive();
            printf("Recv FD %d acquired exclusive mutex\n", a->sockfd);

            if (bytes_recv > 0) {
                printf("Recieved: %s\n", tmp_buffer);
                memcpy((void *) a->msg_buffer, (const void *) tmp_buffer, bytes_recv);
                printf("Wrote received msg to shared buffer.\n");
            } else {
                pthread_mutex_lock(&(a->alive_mutex));    // LOCK
                printf("Recv FD %d acquired alive mutex\n", a->sockfd);
                a->alive = 0;                             // Client no longer alive (i.e disconnected)
                msg_buffer[0] = '\0';
                printf("Client with FD %d disconnected\n", a->sockfd);
                pthread_mutex_unlock(&(a->alive_mutex));  // UNLOCK
                printf("Recv FD %d released alive mutex\n", a->sockfd);
            }

            a->already_sent = 0;

            pthread_mutex_unlock(&(a->a_sent_mutex));
            printf("Recv FD %d released already sent mutex\n", a->sockfd);
            release_exclusive();
            printf("Recv FD %d released exclusive mutex\n", a->sockfd);
        } else {
            break;
        }
    }
    return NULL;
}

void init_client_conns() {
    init_rw_lock();                                 // Initialize the reader writer lock
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
        pthread_mutex_init(&(client_conns[next].a_sent_mutex), NULL);   // Init already sent mutex
        client_conns[next].msg_buffer = msg_buffer;
        client_conns[next].size = MAX_SIZE;
        printf("Added new client with FD %d\n", sockfd);
        pthread_create(&(client_conns[next].send_thread), NULL, send_msg, (void *) &client_conns[next]);
        pthread_create(&(client_conns[next].recv_thread), NULL, recv_msg, (void *) &client_conns[next]);
    }
    return next;
}
