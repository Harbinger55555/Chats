//
// Created by kalesh on 4/24/19.
//

#include "terminal.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

struct termios orig_termios;

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        perror("tcsetattr: ");
        exit(EXIT_FAILURE);
    }
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        perror("tcsetattr: ");
        exit(EXIT_FAILURE);
    }
    // Register disable raw mode to be called at
    // client termination
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        perror("tcsetattr: ");
        exit(EXIT_FAILURE);
    }
}
