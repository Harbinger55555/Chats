//
// Created by kalesh on 4/15/19.
//

#ifndef CHATS_SERVER_CONN_H
#define CHATS_SERVER_CONN_H

#include <sys/socket.h>     // Socket definitions
#include <arpa/inet.h>      // inet functions

#define LISTENQ 1024        // Backlog for listen

int Socket(int domain, int type, int protocol);

void init_server_sockaddr(struct sockaddr_in* serv_addr, char* ip_addr, short port);

void init_client_sockaddr(struct sockaddr_in* serv_addr, char* ip_addr, short port);

void Bind(int list_sock, struct sockaddr* serv_addr, size_t serv_addr_sz);

void Listen(int sock, int backlog);

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

void Connect(int sock, struct sockaddr *servaddr, size_t serv_addr_sz);

void Close(int fd);

#endif //CHATS_SERVER_CONN_H
