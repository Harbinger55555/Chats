//
// Created by Henchhing Limbu on 2019-04-16.
//

#include "message.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>


/*
 * Copies message to buffer in the following format
 *  Msg Buffer: [type|msg|sender|receiver]
 *  Returns size of buffer.
 */
int msgcpy(char *msg_buffer, struct message *msg) {
    uint32_t type = (uint32_t) msg->type;
    uint32_t msg_len = strlen(msg->msg) + 1;
    uint32_t sender_len = strlen(msg->sender) + 1;
    uint32_t receiver_len = strlen(msg->receiver) + 1;

    memcpy((void *) msg_buffer, (const void *) &type, sizeof(uint32_t));
    msg_buffer += sizeof(uint32_t);

    memcpy((void *) msg_buffer, (const void *) &msg_len, sizeof(uint32_t));
    msg_buffer += sizeof(uint32_t);

    memcpy((void *) msg_buffer, (const void *) msg->msg, msg_len);
    msg_buffer += msg_len;

    memcpy((void *) msg_buffer, (const void *) &sender_len, sizeof(uint32_t));
    msg_buffer += sizeof(uint32_t);

    memcpy((void *) msg_buffer, (const void *) msg->sender, sender_len);
    msg_buffer += sender_len;

    memcpy((void *) msg_buffer, (const void *) &receiver_len, sizeof(uint32_t));
    msg_buffer += sizeof(uint32_t);

    memcpy((void *) msg_buffer, (const void *) msg->receiver, receiver_len);

    if (receiver_len <= 1) {
        memcpy((void *) &msg->receiver, (const void *) DEFAULT_RECEIVER,
               strlen(DEFAULT_RECEIVER) + 1);
    }

    return msg_len + sender_len + receiver_len + (4 * sizeof(uint32_t));
}


/* Unpacks incoming message on sockfd into message struct.
 * If return value is not positive then client disconnected.*/
int unpack_msg(int sockfd, struct message *msg) {
    uint32_t type, msg_len, sender_len, receiver_len;

    int bytes_recv = recv(sockfd, (void *) &type, sizeof(uint32_t), 0);
    if (bytes_recv <= 0) {
        return bytes_recv;
    }
    msg->type = (enum msg_type) type;

    bytes_recv = recv(sockfd, (void *) &msg_len, sizeof(uint32_t), 0);
    if (bytes_recv <= 0) {
        return bytes_recv;
    }
    bytes_recv = recv(sockfd, (void *) &msg->msg, msg_len, 0);
    if (bytes_recv <= 0) {
        return bytes_recv;
    }

    bytes_recv = recv(sockfd, (void *) &sender_len, sizeof(uint32_t), 0);
    if (bytes_recv <= 0) {
        return bytes_recv;
    }
    bytes_recv = recv(sockfd, (void *) &msg->sender, sender_len, 0);
    if (bytes_recv <= 0) {
        return bytes_recv;
    }

    bytes_recv = recv(sockfd, (void *) &receiver_len, sizeof(uint32_t), 0);
    if (bytes_recv <= 0) {
        return bytes_recv;
    }
    bytes_recv = recv(sockfd, (void *) &msg->receiver, receiver_len, 0);
    return bytes_recv;
}

