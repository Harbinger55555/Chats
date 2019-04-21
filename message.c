//
// Created by Henchhing Limbu on 2019-04-16.
//

#include "message.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define DEFAULT_RECEIVER "ALL"

/* Creates a message with boundaries */
void create_msg(char* msg_buffer, struct message* msg) {
    if (msg->receiver == NULL) {
        msg->receiver = DEFAULT_RECEIVER;
    }

    uint32_t msg_len = strlen(msg->msg) + 1;
    uint32_t sender_len = strlen(msg->sender) + 1;
    uint32_t receiver_len = strlen(msg->receiver) + 1;

    memcpy((void*) msg_buffer, (const void*) &msg_len, sizeof(uint32_t));
    msg_buffer += sizeof(uint32_t);
    memcpy((void*) msg_buffer, (const void*) msg->msg, msg_len);
    msg_buffer += msg_len;
    memcpy((void*) msg_buffer, (const void*) &sender_len, sizeof(uint32_t));
    msg_buffer += sizeof(uint32_t);
    memcpy((void*) msg_buffer, (const void*) msg->sender, sender_len);
    msg_buffer += sender_len;
    memcpy((void*) msg_buffer, (const void*) &receiver_len, sizeof(uint32_t));
    msg_buffer += sizeof(uint32_t);
    memcpy((void*) msg_buffer, (const void*) msg->receiver, receiver_len);
}

