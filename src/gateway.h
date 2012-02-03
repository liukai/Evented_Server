#ifndef SERVER_GATEWAY
#define SERVER_GATEWAY

#include <stdio.h>
#include <stdlib.h>

const int MAXPENDING = 5;    /* Max connection requests */
const int BUFFSIZE  = 32;

class Gateway {
public:
    Gateway(int port): server_port(port) {};
    ~Gateway() {};

    void run();
private:
    static int setup(int port);
    static void process_requests(int serversock);
    static void handle_client(int clientsock);
    int server_port;
};

inline void die(char *mess) { perror(mess); exit(1); } 

#endif
