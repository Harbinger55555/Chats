//
// Created by kalesh on 4/18/19.
//

#include "client-threads.h"

#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>

#define PROMPT "Enter message to echo: "

volatile char input_buffer[MAX_SIZE];
pthread_mutex_t input_mutex;

void *send_msg(void *args) {
    int sockfd = *((int *) args);
    while (1) {
        printf(PROMPT);
        volatile char *ch = input_buffer;
        while (1) {
            char tmp = getchar();
            pthread_mutex_lock(&input_mutex);
            *ch = tmp;
            if (*ch == '\n') {
                break;
            }
            ch++;
            *ch = '\0';
            pthread_mutex_unlock(&input_mutex);
        }
        *ch = '\0';
        send(sockfd, (void *) input_buffer, strlen((char*) input_buffer) + 1, 0);
        printf("\nSent %s to Server\n", input_buffer);
        *(input_buffer) = '\0';
        pthread_mutex_unlock(&input_mutex);
    }
}

void *recv_msg(void *args) {
    int sockfd = *((int *) args);
    char msg_buffer[MAX_SIZE];
    while (1) {
        if (recv(sockfd, (void *) &msg_buffer, MAX_SIZE, 0) == 0) {
            fprintf(stderr, "Server disconnected\n");
            exit(EXIT_FAILURE);
        }
        // Delete the current line
        pthread_mutex_lock(&input_mutex);
        printf("%c[2K\r", 27);
        printf("Received: %s\n", msg_buffer);
        printf(PROMPT);
        printf("%s", input_buffer);         // TODO: Troubleshoot
        fflush(stdout);
        pthread_mutex_unlock(&input_mutex);
    }
}


void start_client_threads(int sockfd) {
    pthread_mutex_init(&input_mutex, NULL);
    pthread_t send_thread;
    pthread_t recv_thread;
    pthread_create(&send_thread, NULL, send_msg, (void *) &sockfd);
    pthread_create(&recv_thread, NULL, recv_msg, (void *) &sockfd);
    // Important to call join so that sockfd doesn't go out of scope
    // before send / recv threads finish.
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);
}

