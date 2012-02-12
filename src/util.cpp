#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <arpa/inet.h>
#include <fcntl.h>

int set_nonblock(int fd)
{
    int flags;

    flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags);
}

int open_socket(int port) {
    struct sockaddr_in addresslisten;
    int socketlisten = socket(AF_INET, SOCK_STREAM, 0);
    int reuse = 1;
    if (socketlisten < 0)
    {
        fprintf(stderr,"Failed to create listen socket");
        return -1;
    }

    memset(&addresslisten, 0, sizeof(addresslisten));

    addresslisten.sin_family = AF_INET;
    addresslisten.sin_addr.s_addr = INADDR_ANY;
    addresslisten.sin_port = htons(port);

    if (bind(socketlisten, (struct sockaddr *)&addresslisten, sizeof(addresslisten)) < 0)
    {
        fprintf(stderr,"Failed to bind");
        return -1;
    }

    // TODO: backlog size
    if (listen(socketlisten, 500) < 0)
    {
        fprintf(stderr,"Failed to listen to socket");
        return -1;
    }
    set_nonblock(socketlisten);
    setsockopt(socketlisten, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    return socketlisten;
}
int get_accepted_client(int listen_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(listen_fd, 
            (struct sockaddr *)&client_addr,
            &client_len);
    return client_fd;
}
