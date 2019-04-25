#ifndef CHATS_INTERFACE_H
#define CHATS_INTERFACE_H

void get_username(char *namebuffer, int max_size);

int is_command(char *input_buffer);

void parse_command(char *input_buffer);

#endif //CHATS_INTERFACE_H