//
// Created by kalesh on 4/28/19.
//

#include "cmdline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Prints the usage of client
 */
void client_usage() {
    printf("\nUsage: client -a [server ip address] -p [server port number]\n"
           "\n\nIf both server and client are on the same host, no need to specify a port or ip address\n"
           "\t-Usage in this case: client\n"
           "\t-The default port is 2002 and the loopback (127.0.0.1) ip address is used.\n"
           "\n"
           "Options:\n"
           "    -h          Prints this help message\n"
           "    -a IP_ADDR  Flag for specifying server ip address\n"
           "    -p PORT     Flag for specifying the server port\n\n");
}

/*
 * Prints the usage of server
 */
void server_usage() {
    printf("\nUsage: server -p [server port number]\n"
           "\n\nIf both server and client are on the same host, no need to specify a port\n"
           "\t-Usage in this case: server\n"
           "\t-The default port is 2002\n"
           "\n"
           "Options:\n"
           "    -h          Prints this help message\n"
           "    -p PORT     Flag for specifying the server port\n\n");
}

/*
 * Parses the command line and sets the port and ip address, root, and interval.
 */
void parse_cmdline(int argc, char **argv, short *port, char *ip_addr) {
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];        // Get the arg
        if (strcmp(arg, "-h") == 0) {          // Help flag
            if (ip_addr == NULL) {
                server_usage();
            } else {
                client_usage();
            }
            exit(EXIT_FAILURE);
        } else if (strcmp(arg, "-p") == 0) {   // Port flag
            if (++i == argc) {
                fprintf(stderr, "Expected port number following -p flag\n");
                if (ip_addr == NULL) {
                    server_usage();
                } else {
                    client_usage();
                }
                exit(EXIT_FAILURE);
            }
            char *endptr;       // for strtol()
            *port = strtol(argv[i], &endptr, 0);
            if (*endptr) {
                fprintf(stderr, "Invalid port number\n");
                exit(EXIT_FAILURE);
            }
        } else if (ip_addr != NULL && strcmp(arg, "-a") == 0) {   // Ip Address flag
            if (++i == argc) {
                fprintf(stderr, "Expected Ip Address following -a flag\n");
                client_usage();
                exit(EXIT_FAILURE);
            }
            ip_addr = argv[i];
        }
    }
}


