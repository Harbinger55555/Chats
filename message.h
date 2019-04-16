//
// Created by Henchhing Limbu on 2019-04-16.
//

#ifndef CHATS_MESSAGE_H
#define CHATS_MESSAGE_H

# include <unistd.h>

/* Function declaration */
ssize_t read_message(int fd, void *vptr, size_t maxlen);
ssize_t write_message(int fc, const void *vptr, size_t maxlen);

#endif //CHATS_MESSAGE_H
