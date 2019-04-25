//
// Created by kalesh on 4/18/19.
//

#include "client-threads.h"
#include "message.h"
#include "terminal.h"
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>

#define PROMPT "Enter message to echo: "

volatile char input_buffer[MAX_MSG_SIZE];
pthread_mutex_t input_mutex;
struct message send_message;

void *send_msg(void *args) {
    char msg_buffer[MAX_LINE_SIZE];
    int sockfd = *((int *) args);
    while (1) {
        printf(">%s: ", send_message.sender);
        volatile char *ch = input_buffer;

        while (1) {
            char tmp;
            enableRawMode();
            if (read(STDIN_FILENO, &tmp, 1) == 1 && tmp != '\n') {
                disableRawMode();
                pthread_mutex_lock(&input_mutex);
                *ch = tmp;
                if (tmp != '\b') {
                    ch++;
                }
                *ch = '\0';
                pthread_mutex_unlock(&input_mutex);
            } else {
                disableRawMode();
                break;
            }
        }

        pthread_mutex_lock(&input_mutex);
        *ch = '\0';
        pthread_mutex_unlock(&input_mutex);


        // TODO: Check if input is a command
        // printf("\033[2J"); // Clear screen

        memcpy((void *) &(send_message.msg), (const void *) input_buffer, strlen((char *) input_buffer) + 1);
        int buf_len = msgcpy(msg_buffer, &send_message);
        if (strlen(send_message.msg) > 0) {
            send(sockfd, (void *) msg_buffer, buf_len, 0);
        } else {
            //Move cursor up one line
            printf("\033[1A\r"); // Move up X lines;
        }
        *(input_buffer) = '\0';
        pthread_mutex_unlock(&input_mutex);
    }
}

void *recv_msg(void *args) {
    struct message recv_message;
    int sockfd = *((int *) args);
    while (1) {

        int bytes_recv = unpack_msg(sockfd, (&recv_message));
        if (bytes_recv <= 0) {
            fprintf(stderr, "Server disconnected\n");
            exit(EXIT_FAILURE);
        }
        // Delete the current line
        pthread_mutex_lock(&input_mutex);
        printf("%c[2K\r", 27);
        // TODO: Implement this check on the server if time permits
        if (strcmp(recv_message.sender, send_message.sender) != 0) {
            printf("<%s: %s", recv_message.sender, recv_message.msg);
            fflush(stdout);
        }
        printf(">%s: %s", send_message.sender, input_buffer);
        fflush(stdout);

        pthread_mutex_unlock(&input_mutex);
    }
}


void start_client_threads(int sockfd, char *username) {
    pthread_mutex_init(&input_mutex, NULL);
    memcpy((void *) &(send_message.sender), (const void *) username, strlen(username) + 1);
    memcpy((void *) &(send_message.receiver), (const void *) DEFAULT_RECEIVER, strlen(DEFAULT_RECEIVER) + 1);
    pthread_t send_thread;
    pthread_t recv_thread;
    pthread_create(&send_thread, NULL, send_msg, (void *) &sockfd);
    pthread_create(&recv_thread, NULL, recv_msg, (void *) &sockfd);
    // Important to call join so that sockfd doesn't go out of scope
    // before send / recv threads finish.
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);
}

