//
// Created by Henchhing Limbu on 2019-04-16.
//

#ifndef CHATS_MESSAGE_H
#define CHATS_MESSAGE_H

#define MAX_USERNAME_SIZE 31   // Including '\0'  (Only 30 useful)
#define MAX_MSG_SIZE 1001       // Including '\0' (Only 30 useful)
#define MAX_LINE_SIZE MAX_MSG_SIZE + (2 * MAX_USERNAME_SIZE)

struct message {
    char* msg;
    char* sender;
    char* receiver;
};

/*
 * Copies message to buffer in the following format
 *  Msg Buffer: [msg|sender|receiver]
 */
void msgcpy(char* msg_buffer, struct message* msg);

/* Unpacks incoming message on sockfd into message struct.
 * If return value if not positive then client disconnected.*/
int unpack_msg(int sockfd, struct message *msg) {

#endif //CHATS_MESSAGE_H
