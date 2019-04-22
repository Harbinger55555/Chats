//
// Created by kalesh on 4/18/19.
//

#ifndef CHATS_CLIENT_THREADS_H
#define CHATS_CLIENT_THREADS_H

void *send_msg(void *args);

void *recv_msg(void *args);

void start_client_threads(int sockfd, char* username);

#endif //CHATS_CLIENT_THREADS_H
