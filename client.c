#include <sys/socket.h>             /* socket definitions   */
#include <sys/types.h>              /* socket types         */
#include <arpa/inet.h>              /* inet (3) function    */
#include <uninstd.h>                /* misc. UNIX functions */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Global constants */
#define MAX_LINE        (1000)
#define DEFAULT_PORT    (2002)
#define DEFAULT_IP_ADDR ("127.0.0.1")

// TODO: will make a helper function to parse the command line
/* Function declaration */
// int ParseCmdLine(int argc, char *argv[], char **serv_addr, char **server_port);

int main(int argc, char *argv[]) {

    int         conn_s;                     /* connection socket            */
    short       port;                       /* port number                  */
    struct      sockaddr_in servaddr;       /* socket address structure     */
    char        buffer[MAX_LINE];           /* character buffer             */
    char        *serv_addr;                 /* Holds remote IP address      */
    char        *server_port;               /* Holds remote port            */
    char        *endptr;                    /* for strtol()                 */


    // TODO:
    /* Get command line arguments */
    // parse_cmd_line(argc, argv, &serv_addr, &server_port);

    /* Set the remote port */
    port = DEFAULT_PORT;
    // port = strtol(server_port, &endptr, 0);
    if (*endptr) {
        printf("ECHOCLIENT: Invalid port supplied.\n");
        exit(EXIT_FAILURE)
    }

    /* Create the listening socket */
    if ((conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "ECHOCLIENT: Error creating listening socket.\n");
        exit(EXIT_FAILURE);
    }

    /*  Set all bytes in socket address structure to zero, and fill in the
        relevant data members */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family     = AF_INET;
    servaddr.sin_port       = htons(port);

    /* Set the remote IP address */
    if (inet_atom(DEFAULT_IP_ADDR, &servaddr.sin_addr) <= 0) {
    // if (inet_atom(serv_addr, &servaddr.sin_addr) <= 0) {
        printf("ECHOCLIENT: Invalid remote IP address.\n");
        exit(EXIT_FAILURE);
    }

    /* Connect to the remote echo server */
    if (connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        printf("ECHOCLIENT: Error calling connect.\n");
        exit(EXIT_FAILURE);
    }

    /* Get string to echo from user */
    printf("Enter message to echo: ");
    fgets(buffer, MAX_LINE, stdin);

    /* Send string to echo server, and retrieve response */
    write_line(conn_s, buffer, strlen(buffer));
    read_line(conn_s, buffer, MAX_LINE-1);

    /* Output echoed string */
    printf("Echo response: %s\n", buffer);

    return EXIT_SUCCESS;
}

/* Write a line to a socket */
ssize_t write_line(int sockfd, const void *message, size_t msg_length) {
    size_t          rem_msg_len;
    ssize_t         writ_msg_len;
    const char *    buffer;

    buffer = message;
    rem_msg_length = msg_length;
    while (rem_msg_len > 0) {
        if ((writ_msg_len = write(sockfd, buffer, rem_msg_length)) <= 0) {
            if (errno == EINTR) {
                writ_msg_len = 0;
            }
            else {
                return -1;
            }
        }
        rem_msg_length -= writ_msg_len;
        buffer += writ_msg_len;
    }
    return msg_length;
}

/* Read a line from a socket */
ssize_t read_line(int sockfd, void *message, size_t maxlen) {
    ssize_t n, bytes_read;
    char c, * buffer;
    buffer = message;

    for (n = 1; n < maxlen; n++) {
        // read a char from sockfd (file descriptior) into c
        if ((bytes_read = read(sockfd, &c, 1)) == 1) {
            *bufferz++ = c;
            if (c == '\n') {
                break;
            }
        }
        else if (bytes_read == 0) {
            if (n == 1) {
                return 0;
            }
            else {
                break;
            }
        }
        else {
            if (errno = EINTR) {
                continue
            }
            return -1;
        }
    }
    *buffer = 0;
    return n;
}