//
// Created by kalesh on 4/15/19.
//

#include <stdio.h>
#include "connect.h"
#include "server-threads.h"


int main(int argc, char *argv[]) {
    int list_sock;                  // Listening socket
    short port;                     // Port number (default 2002)
    struct sockaddr_in serv_addr;   // Socket address structure

    // TODO: Parse the command line and set the default
    // ports and IP address if no arguments were supplied.
    port = DEFAULT_PORT;

    // Create listening socket
    list_sock = Socket(AF_INET, SOCK_STREAM, 0);

    // Set socket options to allow port an address reuse.
    
    int option = 1;
    setsockopt(list_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    // Initialize the socket address struct
    init_server_sockaddr(&serv_addr, port);

    // Bind socket address to listening socket
    Bind(list_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    // Start listening
    Listen(list_sock, LISTENQ);

    // Initialize the client connections
    init_client_conns();

    // Infinite while loop to respond to client requests
    while (1) {
        // Block until there is a connection request, then accept it
        int sockfd = Accept(list_sock, NULL, NULL);

        // Add the client connection
        if (add_client_conn(sockfd) < 0) {
            fprintf(stderr, "Cannot accept more connections at this time.\n");
            Close(sockfd);
        }
    }
}