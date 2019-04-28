//
// Created by kalesh on 4/24/19.
//

#include "terminal.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

struct termios orig_termios;

void disable_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios) == -1) {
        perror("tcsetattr: ");
        exit(EXIT_FAILURE);
    }
}

void enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        perror("tcsetattr: ");
        exit(EXIT_FAILURE);
    }
    // Register disable raw mode to be called at
    // client termination
    atexit(disable_raw_mode);
    atexit(color_reset);
    struct termios raw = orig_termios;
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ICANON | ECHO);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) {
        perror("tcsetattr: ");
        exit(EXIT_FAILURE);
    }
}

void color_bold_green() {
    printf("\033[1;32m");
}

void color_blue() {
    printf("\033[0;34m");
}

void color_bold_red() {
    printf("\033[1;31m");
}

void color_reset() {
    printf("\033[0m");
}
