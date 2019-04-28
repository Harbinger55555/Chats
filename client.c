//
// Created by Henchhing Limbu on 2019-04-16.
//

#include <sys/socket.h>             // socket definitions
#include <sys/types.h>              // socket types
#include <arpa/inet.h>              // inet (3) function
#include <unistd.h>                 // misc. UNIX functions
#include <stdio.h>
#include <string.h>

#include "message.h"
#include "connect.h"
#include "client-threads.h"
#include "interface.h"

// Global constants
#define DEFAULT_PORT    (2002)
#define DEFAULT_IP_ADDR ("127.0.0.1")

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
    if (len > max_size) {
        printf("Warning! Username truncated to %d characters.", max_size - 1);
    }
    
    // Strip the trailing newline
    char *pos;
    if ((pos = strchr(namebuffer, '\n')) != NULL) {
        *pos = '\0';
    }
}

int main(int argc, char *argv[]) {

    int conn_s;                     // connection socket
    short port;                       // port number
    struct sockaddr_in servaddr;       // socket address structure
    char *ip_addr;                    // server IP address

    // Set the remote port and remote ip address
    port = DEFAULT_PORT;
    ip_addr = DEFAULT_IP_ADDR;

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

