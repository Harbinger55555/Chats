#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "terminal.h"

int is_command(char *input_buffer) {
    return input_buffer[0] == '/';
}

// Returns -1 if no following args or invalid args, else returns 
// index of next character after first whitespace.
int parse_token(char *buffer, char *token) {
	int idx = 0;
	char tmp = buffer[0];
	while (tmp != '\0' && tmp != ' ') {
		++idx;
		tmp = buffer[idx];
	}
	strncpy(token, buffer, idx);
	token[idx++] = '\0';
	// Index of next character after whitespace.
	if (tmp == '\0' || buffer[idx] == '\0') {
		return -1;
	}
	
	// 	Check if it's just all whitespaces, which is considered invalid.
	tmp = buffer[idx];
	while (tmp != '\0') {
		if (tmp != ' ') break;
		++idx;
		tmp = buffer[idx];
	}
	if (tmp == '\0') {
		return -1;
	}
	
	return idx;
}

void help() {
    color_reset();
	printf("For specifying arguments for commands like 'join', angled"
           " brackets should not be included.\n\n");
    printf("    /help                          prints this help display\n");
    printf("    /clear                         clears the screen\n");
    printf("    /quit                          closes the application\n");
    printf("    /rooms                         lists all available rooms\n");
    printf("    /users                         lists all users in the current room\n");
    printf("    /join <room name>              joins the chat room specified"
           " by room name\n");
    printf("    /leave <room name>             leaves the chat room specified"
           " by room name\n");
    printf("    /dm <user name> <message>      sends the message to the user"
           " specified by user name\n");
}

void clear_screen() {
    // Clears the screen.
    printf("\033[2J");
    printf("%c[H\r", 27);
    fflush(stdout);
}

void quit() {
    exit(0);
}

void join(char *input_buffer, int args_idx, struct message *send_message) {
	if (args_idx < 0) {
        color_bold_red();
		printf("Missing room name!\n");
		return;
	}
	char *msg = input_buffer + args_idx;
	send_message->type = JOIN;
	memcpy((void *) &(send_message->msg), (const void *) (msg), strlen((char *) msg) + 1);
}

void leave(char *input_buffer, int args_idx, struct message *send_message) {
	if (args_idx < 0) {
        color_bold_red();
		printf("Missing room name!\n");
		return;
	}
	char *msg = input_buffer + args_idx;
	send_message->type = LEAVE;
	memcpy((void *) &(send_message->msg), (const void *) (msg), strlen((char *) msg) + 1);
}

void rooms(struct message *send_message) {
	send_message->type = ROOMS;
    char *dummy = " \0"; // To send command to server.
    memcpy((void *) &(send_message->msg), (const void *) (dummy), strlen((char *) dummy) + 1);
}

void users(struct message *send_message) {
	send_message->type = USERS;
    char *dummy = " \0"; // To send command to server.
    memcpy((void *) &(send_message->msg), (const void *) (dummy), strlen((char *) dummy) + 1);
}

void dm(char *input_buffer, int args_idx, struct message *send_message) {
	if (args_idx < 0) {
        color_bold_red();
		printf("Missing receiver!\n");
		return;
	}
	char *args = input_buffer + args_idx;
	send_message->type = DM;
	
	// User could enter a continous invalid command with MAX_LINE_SIZE.
	char receiver[MAX_LINE_SIZE];
	int msg_idx = parse_token(args, receiver);
	if (msg_idx < 0) {
        color_bold_red();
		printf("Missing message!\n");
		return;
	} 
	
	memcpy((void *) &(send_message->receiver), (const void *) (receiver), strlen((char *) receiver) + 1);
	char *msg = args + msg_idx;
	memcpy((void *) &(send_message->msg), (const void *) (msg), strlen((char *) msg) + 1);
}

void command_action(char *input_buffer, struct message *send_message) {
	// User could enter a continous invalid command with MAX_LINE_SIZE.
	char cmd[MAX_LINE_SIZE]; 
	int idx = parse_token(input_buffer, cmd);
	send_message->msg[0] = '\0'; // Clear send_message->msg first.
	
	if (strcmp(cmd, "help") == 0) {
        help();
    }
	else if (strcmp(cmd, "clear") == 0) {
        clear_screen();
    }
	else if (strcmp(cmd, "quit") == 0) {
        quit();
    }
	else if (strcmp(cmd, "join") == 0) {
		join(input_buffer, idx, send_message);
    }
	else if (strcmp(cmd, "leave") == 0) {
		leave(input_buffer, idx, send_message);
    }
	else if (strcmp(cmd, "rooms") == 0) {
        rooms(send_message);
    }
	else if (strcmp(cmd, "users") == 0) {
        users(send_message);
    }
	else if (strcmp(cmd, "dm") == 0) {
        dm(input_buffer, idx, send_message);
    }
	else {
        color_bold_red();
		printf("Invalid command!\n");
	}
}
