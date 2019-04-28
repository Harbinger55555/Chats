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

// Global constants
#define DEFAULT_PORT    (2002)
#define DEFAULT_IP_ADDR ("127.0.0.1")

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

    char username[MAX_USERNAME_SIZE];

    printf("\033[1;32m");       // Set the color to bold green
    printf("Please enter your user name: ");

    fflush(stdout);

    printf("\033[0;34m");       // Set the color to blue
    fgets(username, MAX_USERNAME_SIZE, stdin);
    printf("\033[0m");          //Resets the text to default color

    // Strip the trailing newline
    char *pos;
    if ((pos = strchr(username, '\n')) != NULL) {
        *pos = '\0';
    }

    start_client_threads(conn_s, username);
}

