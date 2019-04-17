//
// Created by kalesh on 4/15/19.
//

#include <stdio.h>
#include "connection.h"
#include "message.h"

// Global constants
#define MAX_SIZE 1000
#define DEFAULT_PORT 2002
#define DEFAULT_IP_ADDR "127.0.0.1"


int main(int argc, char* argv[]) {
    int list_sock;                  // Listening socket
//    int conn_sock;                  // Connection socket
    short port;                     // Port number (default 2002)
    char* ip_addr;                  // Ip Address (default "127.0.0.1")
    struct sockaddr_in serv_addr;   // Socket address structure
    char msg_buffer[MAX_SIZE];      // Message buffer
    struct client_conn client_conns[MAX_CLIENT_CONN];
    int client_count = 0;

    // Initialize the mutex
    pthread_mutex_t msg_mutex = PTHREAD_MUTEX_INITIALIZER;

    // TODO: Parse the command line and set the default
    // ports and IP address if no arguments were supplied.
    port = DEFAULT_PORT;
    ip_addr = DEFAULT_IP_ADDR;

    // Create listening socket
    list_sock = Socket(AF_INET, SOCK_STREAM, 0);

    // Initialize the socket address struct
    init_sockaddr(&serv_addr, ip_addr, port);

    // Bind socket address to listening socket
    Bind(list_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    // Start listening
    Listen(list_sock);

    // Infinite while loop to respond to client requests
    /*while (1) {
        // Block until there is a connection request, then accept it
        conn_sock = Accept(list_sock, NULL, NULL);

        // Receive a message from the connected socket
        read_message(conn_sock, (void*) &msg_buffer, MAX_SIZE);

        // Write (echo) the message back to the same socket.
        write_message(conn_sock, (void*) &msg_buffer, MAX_SIZE);

        Close(conn_sock);
    }*/
    while (client_count < MAX_CLIENT_CONN) {
        int sock_fd = Accept(list_sock, NULL, NULL);
        client_conns[client_count].sock_fd = sock_fd;
        struct thread_args args = {sock_fd, msg_buffer, MAX_SIZE, &msg_mutex};
        client_count++;
        pthread_create(&(client_conns[client_count].thread), NULL, echo, (void*) &args);
    }

    for (int i = 0; i < MAX_CLIENT_CONN; i++) {
        pthread_join(client_conns[i].thread, NULL);
        Close(client_conns[i].sock_fd);
    }
}