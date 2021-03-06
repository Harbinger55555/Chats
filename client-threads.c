//
// Created by kalesh on 4/18/19.
//

#include "client-threads.h"
#include "message.h"
#include "interface.h"
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
        color_bold_green();
        printf("%s%s: ", OUT_PROMPT, send_message.sender);
        color_blue();

        fflush(stdout);

        i = 0;
        while (1) {
            enable_raw_mode();
            tmp = getchar();
            disable_raw_mode();
            pthread_mutex_lock(&input_mutex);
            if (tmp == '\n' || tmp == '\r') {              // Enter key pressed
                // Remove the trailing newline
                input_buffer[i] = '\0';
                // Move cursor down one line
                printf("\r\n");
                fflush(stdout);
                break;
            } else if ((int) tmp == 127 && i > 0) {         // Backspace key pressed
                // Remove the last printed character on the terminal
                printf("\b \b");

                // Replace the last char with the end of line
                input_buffer[--i] = '\0';
                fflush(stdout);
            } else if (tmp == '\033') {                     // Escape sequence (ignore)
                getchar();
                getchar();
            } else if (tmp != 127) {
                    printf("%c", tmp);
                    input_buffer[i] = tmp;
                    i++;
                    input_buffer[i] = '\0';
            }
            fflush(stdout);
            pthread_mutex_unlock(&input_mutex);
        }

        if (is_command(input_buffer)) {
            command_action(input_buffer + 1, &send_message);
        } else { // Input is a message
            // Set message type
            send_message.type = MSG;
            memcpy((void *) &(send_message.msg), (const void *) input_buffer, strlen((char *) input_buffer) + 1);
        }

        int buf_len = msgcpy(msg_buffer, &send_message);
        if (strlen(send_message.msg) > 0) {
            send(sockfd, (void *) msg_buffer, buf_len, 0);
        } else if (!is_command(input_buffer)) {
            // Don't allow the user to move off of the
            // screen by entering empty lines
            printf("\033[1A\r"); // Move up 1 lines;
            printf("\033[2K\r"); // Clear line and moves cursor back to start.
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

            printf("\033[2K\r");
            color_bold_red();
            printf("Server disconnected\r\n");

            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&input_mutex);
        // Delete the current line
        printf("\033[2K\r");
        // Only display the message if it's from someone else
        if (strcmp(recv_message.sender, send_message.sender) != 0) {

            color_bold_green();
            
            // For dm message
            if (recv_message.type == DM) {
                printf("%s%s(DM): ", IN_PROMPT, recv_message.sender);
            }
            else {
                printf("%s%s: ", IN_PROMPT, recv_message.sender);
            }

            color_red();
            printf("%s\r\n", recv_message.msg);
        }
        // Reprint the out prompt and the input buffer

        color_bold_green();
        printf("%s%s: ", OUT_PROMPT, send_message.sender);

        color_blue();
        printf("%s", input_buffer);
        fflush(stdout);
        pthread_mutex_unlock(&input_mutex);
    }
}

void *send_username(void *args) {
    char msg_buffer[MAX_LINE_SIZE];
    int sockfd = *((int *) args);

    pthread_mutex_lock(&input_mutex);
    // Set message type
    send_message.type = USERNAME;
    char *username_msg = "username";
    memcpy((void *) &(send_message.msg), (const void *) username_msg, strlen((char *) username_msg) + 1);
    int buf_len = msgcpy(msg_buffer, &send_message);
    send(sockfd, (void *) msg_buffer, buf_len, 0);
    pthread_mutex_unlock(&input_mutex);

    return NULL;
}


void start_client_threads(int sockfd, char *username) {
    pthread_mutex_init(&input_mutex, NULL);
    memcpy((void *) &(send_message.sender), (const void *) username, strlen(username) + 1);
    memcpy((void *) &(send_message.receiver), (const void *) DEFAULT_RECEIVER, strlen(DEFAULT_RECEIVER) + 1);
    pthread_t send_thread;
    pthread_t recv_thread;
    pthread_t username_thread;

    pthread_create(&username_thread, NULL, send_username, (void *) &sockfd);
    pthread_create(&send_thread, NULL, send_msg, (void *) &sockfd);
    pthread_create(&recv_thread, NULL, recv_msg, (void *) &sockfd);
    // Important to call join so that sockfd doesn't go out of scope
    // before send / recv threads finish.
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);
}

