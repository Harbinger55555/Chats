//
// Created by Henchhing Limbu on 2019-04-16.
//

#include <sys/socket.h>             // socket definitions
#include <sys/types.h>              // socket types
#include <arpa/inet.h>              // inet (3) function
#include <unistd.h>                 // misc. UNIX functions
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "cmdline.h"
#include "message.h"
#include "connect.h"
#include "client-threads.h"
#include "interface.h"
#include "terminal.h"


int name_len(char *a, int max_size) {
    int len = 0;
    for (int i = 0; i < max_size; i++) {
        if (a[i] == '\n') break;
        len++;
    }
    return len;
}

void get_username(char *namebuffer, int max_size) {
    color_bold_green();
    printf("Please enter your user name: ");
    fflush(stdout);
    color_blue();
    
    fgets(namebuffer, max_size, stdin);
    color_reset();
    
    int len = name_len(namebuffer, max_size);
    // 16 to account for the '\n' at the end.
    if (len > max_size) {
        printf("Warning! Username truncated to %d characters.", max_size - 1);
    }
    
    // Strip the trailing newline
    char *pos;
    if ((pos = strchr(namebuffer, '\n')) != NULL) {
        *pos = '\0';
    }
}

void terminal_color_reset(int sig) {
    color_reset();
    fflush(stdout);
    signal(SIGINT, SIG_DFL); // Call default SIGINT handler.
    raise(SIGINT);
}

int main(int argc, char *argv[]) {

    int conn_s;                     // connection socket
    short port;                       // port number
    struct sockaddr_in servaddr;       // socket address structure
    char *ip_addr;                    // server IP address
    
    signal(SIGINT, terminal_color_reset);  // Resets terminal color if SIGINT received. 
    signal(SIGTSTP, terminal_color_reset);  // Resets terminal color if SIGTSTP received.

    // Set the remote port and remote ip address
    port = DEFAULT_PORT;
    ip_addr = DEFAULT_IP_ADDR;
    parse_cmdline(argc, argv, &port, ip_addr);

    // Create the listening socket
    conn_s = Socket(AF_INET, SOCK_STREAM, 0);

    //  Set all bytes in socket address structure to zero, and fill in the
    //    relevant data members
    init_client_sockaddr(&servaddr, ip_addr, port);

    // Connect to the remote echo server
    Connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr));

    // Get username.
    char username[MAX_USERNAME_SIZE];
    get_username(username, MAX_USERNAME_SIZE);

    start_client_threads(conn_s, username);
}

