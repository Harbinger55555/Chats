//
// Created by Henchhing Limbu on 2019-04-16.
//

#include <sys/socket.h>             // socket definitions
#include <sys/types.h>              // socket types
#include <arpa/inet.h>              // inet (3) function
#include <unistd.h>                 // misc. UNIX functions

#include "message.h"
#include "connect.h"
#include "client-threads.h"

// Global constants
#define MAX_LINE        (1000)
#define DEFAULT_PORT    (2002)
#define DEFAULT_IP_ADDR ("127.0.0.1")

int main(int argc, char *argv[]) {

    int         conn_s;                     // connection socket
    short       port;                       // port number
    struct      sockaddr_in servaddr;       // socket address structure
//    char        buffer[MAX_LINE];           // character buffer
    char*       ip_addr;                    // server IP address

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

    // TODO: Get username;

    start_client_threads(conn_s);
    /*
    while(1) {
        // Get string to echo from user
        printf("Enter message to echo: ");
        fgets(buffer, MAX_LINE, stdin);

        // Send string to echo server, and retrieve response
        write_message(conn_s, buffer, strlen(buffer));
        read_message(conn_s, buffer, MAX_LINE - 1);

        // Output echoed string
        printf("Echo response: %s\n", buffer);
    }
     */
}

