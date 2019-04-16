//
// Created by Henchhing Limbu on 2019-04-16.
//

#include "message.h"
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

/* Write a message to a socket */
ssize_t write_message(int sockfd, const void *message, size_t msg_length) {
    size_t          rem_msg_len;
    ssize_t         writ_msg_len;
    const char *    buffer;

    buffer = message;
    rem_msg_len = msg_length;
    while (rem_msg_len > 0) {
        if ((writ_msg_len = write(sockfd, buffer, rem_msg_len)) <= 0) {
            if (errno == EINTR) {
                writ_msg_len = 0;
            }
            else {
                return -1;
            }
        }
        rem_msg_len -= writ_msg_len;
        buffer += writ_msg_len;
    }
    return msg_length;
}

/* Read a message from a socket */
ssize_t read_message(int sockfd, void *message, size_t maxlen) {
    ssize_t n, bytes_read;
    char c, * buffer;
    buffer = message;

    for (n = 1; n < maxlen; n++) {
        // read a char from sockfd (file descriptior) into c
        if ((bytes_read = read(sockfd, &c, 1)) == 1) {
            *buffer++ = c;
            if (c == '\n') {
                break;
            }
        }
        else if (bytes_read == 0) {
            if (n == 1) {
                return 0;
            }
            else {
                break;
            }
        }
        else {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
    }
    *buffer = 0;
    return n;
}
