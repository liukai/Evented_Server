#include "gateway.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <event2/event.h>

void Gateway::run() {
    int serversock = setup(server_port);
    process_requests(serversock);
}
int Gateway::setup(int port) {
    int serversock;
    struct sockaddr_in echoserver, echoclient;

    /* Create the TCP socket */
    if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        die("Failed to create socket");
    }

    /* Construct the server sockaddr_in structure */
    memset(&echoserver, 0, sizeof(echoserver));       /* Clear struct */
    echoserver.sin_family = AF_INET;                  /* Internet/IP */
    echoserver.sin_addr.s_addr = htonl(INADDR_ANY);   /* Incoming addr */
    echoserver.sin_port = htons(port);       /* server port */

    /* Bind the server socket */
    if (bind(serversock, (struct sockaddr *) &echoserver,
                sizeof(echoserver)) < 0) {
        die("Failed to bind the server socket");
    }

    return serversock;
}
void Gateway::process_requests(int serversock) {
    int clientsock;
    struct sockaddr_in echoclient;

    /* Listen on the server socket */
    if (listen(serversock, MAXPENDING) < 0) {
        die("Failed to listen on server socket");
    } 
    /* Run until cancelled */
    while (1) {
        unsigned int clientlen = sizeof(echoclient);
        /* Wait for client connection */
        clientsock = accept(serversock, (struct sockaddr *) &echoclient, &clientlen);
        if (clientsock < 0) {
            die("Failed to accept client connection");
        }
        fprintf(stdout, "Client connected: %s\n", inet_ntoa(echoclient.sin_addr));

        // TODO: hot point
        handle_client(clientsock);
    }
}

void kqueue_test() {

}


void Gateway::handle_client(int sock) {
    char buffer[BUFFSIZE];
    char precede[] = "From server> ";
    int received = -1;
    /* Receive message */
    if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
        die("Failed to receive initial bytes from client");
    }
    /* Send bytes and check for more incoming data in loop */
    while (received > 0) {
        /* Send back received data */
        if (send(sock, precede, sizeof(precede), 0) != sizeof(precede)) {
            die("Failed to send bytes to client");
        }
        if (send(sock, buffer, received, 0) != received) {
            die("Failed to send bytes to client");
        }
        /* Check for more data */
        if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
            die("Failed to receive additional bytes from client");
        }
    }
    close(sock);
}

