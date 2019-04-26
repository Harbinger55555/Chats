#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"

int name_len(char *a, int max_size) {
    int len = 0;
    for (int i = 0; i < max_size; i++) {
        if (a[i] == '\n') break;
        len++;
    }
    return len;
}

void get_username(char *namebuffer, int max_size) {
    printf("Please enter your user name: ");
    fflush(stdout);
    fgets(namebuffer, max_size, stdin);
    
    int len = name_len(namebuffer, max_size);
    // 16 to account for the '\n' at the end.
    if (len > 16) {
        printf("Warning! Username truncated to %d characters.", max_size - 1);
    }
    
    // Strip the trailing newline
    char *pos;
    if ((pos = strchr(namebuffer, '\n')) != NULL) {
        *pos = '\0';
    }
}

void clear_screen() {
    // Clears the screen.
    printf("\033[2J");
    printf("%c[H\r", 27);
    fflush(stdout);
}

void join() {
    
}

void quit() {
    exit(0);
}

int is_command(char *input_buffer) {
    return input_buffer[0] == '/';
}

void parse_command(char *input_buffer) {
    if (strcmp(input_buffer + 1, "clear") == 0) {
        clear_screen();
    }
    else if (strcmp(input_buffer + 1, "join") == 0) {
        join();
    }
    else if (strcmp(input_buffer + 1, "quit") == 0) {
        quit();
    }
    else {
        printf("Invalid command!\n");
    }
}


