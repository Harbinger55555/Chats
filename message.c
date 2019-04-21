//
// Created by Henchhing Limbu on 2019-04-16.
//

#include "message.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define DEFAULT_RECEIVER "ALL"

/*
 * Copies message to buffer in the following format
 *  Msg Buffer: [msg|sender|receiver]
 */
void msgcpy(char *msg_buffer, struct message *msg) {
    if (msg->receiver == NULL) {
        msg->receiver = DEFAULT_RECEIVER;
    }

    uint32_t msg_len = strlen(msg->msg) + 1;
    uint32_t sender_len = strlen(msg->sender) + 1;
    uint32_t receiver_len = strlen(msg->receiver) + 1;

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
}


/* Unpacks incoming message on sockfd into message struct.
 * If return value if not positive then client disconnected.*/
int unpack_msg(int sockfd, struct message *msg) {
    uint32_t msg_len, sender_len, receiver_len;

    int bytes_recv = recv(sockfd, (void *) &msg_len, sizeof(uint32_t), 0);
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



