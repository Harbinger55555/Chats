//
// Created by kalesh on 4/15/19.
//

#include "connect.h"

#include <string.h>         // memset()
#include <stdio.h>          // perror()
#include <stdlib.h>         // EXIT_FAILURE

#include <sys/types.h>      // Socket types
#include <sys/socket.h>
#include <netinet/in.h>     // inet_aton()
#include <arpa/inet.h>      // inet functions
#include <unistd.h>         // close()


int Socket(int domain, int type, int protocol) {
    int sock_fd;
    if ((sock_fd = socket(domain, type, protocol)) < 0) {
        perror("socket: ");
        exit(EXIT_FAILURE);
    }
    return sock_fd;
}

void init_server_sockaddr(struct sockaddr_in* serv_addr, char* ip_addr, short port) {
    memset(serv_addr, 0, sizeof(*serv_addr));
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port =htons((uint16_t) port);
    struct in_addr in;
    if (inet_aton(ip_addr, &in) == 0) {
        fprintf(stderr, "Invalid IP address provided, binding socket to all available local interfaces.\n");
        serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        serv_addr->sin_addr.s_addr = in.s_addr;
    }
}

void init_client_sockaddr(struct sockaddr_in* serv_addr, char* ip_addr, short port) {
    memset(serv_addr, 0, sizeof(*serv_addr));
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port =htons((uint16_t) port);
    struct in_addr in;
    if (inet_aton(ip_addr, &in) <= 0) {
        fprintf(stderr, "Invalid IP address provided\n");
        exit(EXIT_FAILURE);
    } else {
        serv_addr->sin_addr.s_addr = in.s_addr;
    }
}

void Bind(int list_sock, struct sockaddr* serv_addr, size_t serv_addr_sz) {
    if (bind(list_sock, serv_addr, (socklen_t) serv_addr_sz) < 0) {
        perror("bind: ");
        exit(EXIT_FAILURE);
    }
}

void Listen(int sock, int backlog) {
    if (listen(sock, backlog) < 0) {
        perror("listen: ");
        exit(EXIT_FAILURE);
    }
}

void Connect(int sock, struct sockaddr *servaddr, size_t serv_addr_sz) {
    if (connect(sock, servaddr, serv_addr_sz) < 0) {
        perror("connect: ");
        exit(EXIT_FAILURE);
    }
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int conn_fd;
    if ((conn_fd = accept(sockfd, addr, addrlen)) < 0) {
        perror("accept: ");
        exit(EXIT_FAILURE);
    }
    return conn_fd;
}

void Close(int fd) {
    if (close(fd) < 0) {
        perror(("close: "));
        exit(EXIT_FAILURE);
    }
}