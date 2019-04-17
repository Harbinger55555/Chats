//
// Created by kalesh on 4/15/19.
//

#ifndef CHATS_SERVER_CONN_H
#define CHATS_SERVER_CONN_H

#include <sys/socket.h>         // Socket definitions
#include <arpa/inet.h>          // inet functions
#include <pthread.h>            // pthreads

#define LISTENQ 1024            // Backlog for listen
#define MAX_CLIENT_CONN 1000    // Max client connections

struct client_conn {
    int sock_fd;
    pthread_t thread;
};

int Socket(int domain, int type, int protocol);

void init_sockaddr(struct sockaddr_in* serv_addr, char* ip_addr, short port);

void Bind(int list_sock, struct sockaddr* serv_addr, size_t serv_addr_sz);

void Listen(int sock);

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);


void Close(int fd);

#endif //CHATS_SERVER_CONN_H
