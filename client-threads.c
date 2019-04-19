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

char input_buffer[MAX_SIZE];
int input_count;

void *send_msg(void *args) {
    int sockfd = *((int *) args);
    while (1) {
        printf("Enter message to echo: ");
//        fgets(msg_buffer, MAX_SIZE, stdin);
        char *ch = input_buffer;
        input_count = 0;
        while ((*ch = getchar()) != '\n') {
            ch++;
            input_count++;
        }
        *ch = '\0';
        send(sockfd, (void *) input_buffer, strlen(input_buffer) + 1, 0);
        printf("\nSent %s to Server\n", input_buffer);
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
        // TODO: Delete the current line
        printf("%c[2K\r", 27);
        printf("Received: %s\n", msg_buffer);
        printf("Input count: %d\n", input_count);
        printf("Enter message to echo: ");
        fflush(stdout);
        char* ch = input_buffer;
        for (int i = 0; i < input_count; i++) {
            putc(*ch, stdout);
            ch++;
        }
        fflush(stdout);
    }
}


void start_client_threads(int sockfd) {
    pthread_t send_thread;
    pthread_t recv_thread;
    pthread_create(&send_thread, NULL, send_msg, (void *) &sockfd);
    pthread_create(&recv_thread, NULL, recv_msg, (void *) &sockfd);
    // Important to call join so that sockfd doesn't go out of scope
    // before send / recv threads finish.
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);
}

