//
// Created by Henchhing Limbu on 2019-04-16.
//

#ifndef CHATS_MESSAGE_H
#define CHATS_MESSAGE_H

struct message {
    char* msg;
    char* sender;
    char* receiver;
};


/* Creates a message with boundaries */
void create_msg(char* msg_buffer, struct message* msg);

#endif //CHATS_MESSAGE_H
