//
// Created by Henchhing Limbu on 2019-04-16.
//

#ifndef CHATS_MESSAGE_H
#define CHATS_MESSAGE_H

#include <unistd.h>
#include <pthread.h>            // pthreads

/* Function declaration */
ssize_t read_message(int fd, void *vptr, size_t maxlen);
ssize_t write_message(int fc, const void *vptr, size_t maxlen);

struct thread_args {
    int sock_fd;
    char* buffer;
    size_t size;
    pthread_mutex_t* msg_mutex;
};

void* read_message_t(void* args);
void* write_message_t(void* args);

void* echo(void* args);

#endif //CHATS_MESSAGE_H
