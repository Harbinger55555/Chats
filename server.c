//
// Created by kalesh on 4/15/19.
//

#include <stdio.h>
#include "connect.h"
#include "server-threads.h"
#include "cmdline.h"
#include <ifaddrs.h>

void server_info(short port) {

    struct ifaddrs *addrs;
    struct ifaddrs *tmp;

    getifaddrs(&addrs);
    tmp = addrs;
    printf("Server Network Interfaces and IP Addresses:\n");
    while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            printf("\t-%s: %s\n", tmp->ifa_name, inet_ntoa(pAddr->sin_addr));
        }
        tmp = tmp->ifa_next;
    }

    freeifaddrs(addrs);
    printf("Server Port Number: %d\n", port);
}


int main(int argc, char *argv[]) {
    int list_sock;                  // Listening socket
    short port;                     // Port number (default 2002)
    struct sockaddr_in serv_addr;   // Socket address structure

    port = DEFAULT_PORT;

    parse_cmdline(argc, argv, &port, NULL);


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

    // Print server info
    server_info(port);

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