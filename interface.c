#include <stdio.h>
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
    // 16 to account for the '\0' at the end.
    if (len > 16) {
        printf("Warning! Username truncated to 15 characters.");
    }
    
    // Strip the trailing newline
    char *pos;
    if ((pos = strchr(namebuffer, '\n')) != NULL) {
        *pos = '\0';
    }
}