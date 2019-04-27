//
// Created by Henchhing Limbu on 2019-04-16.
//

#ifndef CHATS_MESSAGE_H
#define CHATS_MESSAGE_H

# include <unistd.h>

#define MAX_USERNAME_SIZE 31   // Including '\0'  (Only 30 useful)
#define MAX_MSG_SIZE 1001       // Including '\0' (Only 30 useful)
#define MAX_LINE_SIZE MAX_MSG_SIZE + (2 * MAX_USERNAME_SIZE)
#define DEFAULT_RECEIVER "all"
#define IN_PROMPT ">"
#define OUT_PROMPT "<"

enum msg_type {
    MSG,        // Normal message (Not a command)

    // Server Side Commands:
    JOIN,       /*  /join [chat room]           */
    LEAVE,      /*  /leave                      */
    DM,         /*  /dm [@username] [message]   */
    ROOMS,      /*  /rooms                      */
    USERS,      /*  /users                      */

    // Special Command
    USERNAME    // Sends user name to server

    /*
     * Client Side Commands:
     * CLEAR        /clear
     * QUIT         /quit
     * HELP         /help
     * */
};

struct message {
    enum msg_type  type;
    char msg[MAX_LINE_SIZE];
    char sender[MAX_USERNAME_SIZE];
    char receiver[MAX_USERNAME_SIZE];
};

/*
 * Copies message to buffer in the following format
 *  Msg Buffer: [msg|sender|receiver]
 *  Returns the size of the buffer.
 */
int msgcpy(char *msg_buffer, struct message *msg);

/* Unpacks incoming message on sockfd into message struct.
 * If return value is not positive then client disconnected.*/
int unpack_msg(int sockfd, struct message *msg);

#endif //CHATS_MESSAGE_H
