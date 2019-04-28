#ifndef CHATS_INTERFACE_H
#define CHATS_INTERFACE_H

#include "message.h"

int is_command(char *input_buffer);

void command_action(char *input_buffer, struct message *send_message);

#endif //CHATS_INTERFACE_H