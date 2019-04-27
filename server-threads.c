//
// Created by kalesh on 4/16/19.
//

#include "server-threads.h"
#include "connect.h"

char msg_buffer[MAX_LINE_SIZE];                                  // Message buffer
struct client_conn client_conns[MAX_CLIENT_CONNS];          // Client connections
pthread_mutex_t mutex;      // Mutex for message buffer

void *send_msg(void *args) {
    struct client_conn *a = (struct client_conn *) args;
    // Don't send empty messages
    if (strlen(msg_buffer + sizeof(uint32_t)) > 0) {
        void *buffer = (void *) msg_buffer;
        send(a->sockfd, buffer, sizeof(uint32_t), 0);
        buffer += sizeof(uint32_t);
        for (int i = 0; i < 3; i++) {
            send(a->sockfd, buffer, sizeof(uint32_t), 0);
            buffer += sizeof(uint32_t);
            send(a->sockfd, buffer, strlen((char *) buffer) + 1, 0);
            buffer += strlen((char *) buffer) + 1;
        }
    }
    return NULL;
}

void *recv_msg(void *args) {
    struct client_conn *a = (struct client_conn *) args;

    // No need to lock as only this thread can change the value of
    // the alive variable.
    while (a->alive == 1) {
        int bytes_recv = unpack_msg(a->sockfd, (&a->msg));
        pthread_mutex_lock(&mutex);

        if (bytes_recv > 0) {
            printf("%s: %s\n", a->msg.sender, a->msg.msg);
            msgcpy(msg_buffer, &a->msg);

            if (a->msg.type == MSG) {
                // Send message to everyone in the chat room
                for (int i = 0; i < MAX_CLIENT_CONNS; i++) {
                    if (client_conns[i].alive == 1) {
                        pthread_create(&(client_conns[i].send_thread), NULL, send_msg, (void *) &client_conns[i]);
                    }
                }

                // Wait for the message to be sent to everyone before
                // allowing another receiver thread to overwrite the msg_buffer.
                for (int i = 0; i < MAX_CLIENT_CONNS; i++) {
                    if (client_conns[i].alive == 1) {
                        pthread_join(client_conns[i].send_thread, NULL);
                    }
                }
            } else if (a->msg.type == DM) {
                // Direct message
                for (int i = 0; i < MAX_CLIENT_CONNS; i++) {
                    if (client_conns[i].alive == 1 && strcmp(client_conns[i].name, a->msg.receiver) == 0) {
                        pthread_create(&(client_conns[i].send_thread), NULL, send_msg, (void *) &client_conns[i]);
                        pthread_join(client_conns[i].send_thread, NULL);
                        break;
                    }
                }
            } else if (a->msg.type == USERNAME) {
                // Set the username of the client connection
                memcpy(a->name, a->msg.sender, strlen(a->msg.sender) + 1);
            }

            // TODO: Handle different message types here

        } else {
            a->alive = 0;       // Set alive to 0
            printf("Client with FD %d disconnected\n", a->sockfd);
            Close(a->sockfd);                                 // Close the connection socket
            printf("Closed FD %d\n", a->sockfd);
        }

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void init_client_conns() {
    pthread_mutex_init(&mutex, NULL);   // Initialize the mutex
    for (int i = 0; i < MAX_CLIENT_CONNS; i++) {    // All connections are initially not alive
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
        pthread_create(&(client_conns[next].recv_thread), NULL, recv_msg, (void *) &client_conns[next]);
        printf("Added new client with FD %d\n", sockfd);
    }
    return next;
}
