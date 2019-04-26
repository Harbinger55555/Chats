//
// Created by kalesh on 4/18/19.
//

#include "client-threads.h"
#include "message.h"
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include "terminal.h"

char input_buffer[MAX_MSG_SIZE] = {0};
pthread_mutex_t input_mutex;
struct message send_message;

void *send_msg(void *args) {
    char msg_buffer[MAX_LINE_SIZE];
    int sockfd = *((int *) args);
    int i;
    char tmp;
    while (1) {
        printf(">%s: ", send_message.sender);
        
        i = 0;
        while (1) {
            enableRawMode();
            tmp = getchar();
            disableRawMode();
            pthread_mutex_lock(&input_mutex);
            if (tmp == '\n' || tmp == '\r') {              // Enter key pressed
                // Remove the trailing newline
                input_buffer[i] = '\0';
                // Move cursor down one line
                printf("\r\n");
                fflush(stdout);
                break;
            }
            if ((int) tmp == 127 && i > 0) {     // Backspace
                // Replace the last char with the end of line
                input_buffer[--i] = '\0';
                // Remove the last printed character on the terminal
                printf("\b \b");
                fflush(stdout);
            } else {
                printf("%c", tmp);
                input_buffer[i] = tmp;
                i++;
                input_buffer[i] = '\0';
            }
            fflush(stdout);
            pthread_mutex_unlock(&input_mutex);
        }

        // TODO: Check if input is a command
        // printf("\033[2J"); // Clear screen

        memcpy((void *) &(send_message.msg), (const void *) input_buffer, strlen((char *) input_buffer) + 1);
        int buf_len = msgcpy(msg_buffer, &send_message);
        if (strlen(send_message.msg) > 0) {
            send(sockfd, (void *) msg_buffer, buf_len, 0);
        } else {
            // Don't allow the user to move off of the 
            // screen by entering empty lines
            printf("\033[1A\r"); // Move up 1 lines;
        }
        input_buffer[0] = '\0';

        // We broke out of the while loop
        // without unlocking
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
   
        pthread_mutex_lock(&input_mutex);
        // Delete the current line
        printf("\033[2K\r");
        // Only display the message if it's from someone else
        if (strcmp(recv_message.sender, send_message.sender) != 0) {
            printf("<%s: %s\r\n", recv_message.sender, recv_message.msg);
        }
        // Reprint the prompt and the input buffer
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

