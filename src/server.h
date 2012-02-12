#ifndef IS_EVHTTP_SERVER_H
#define IS_EVHTTP_SERVER_H

#include <event.h>
#include "config.h"

class ClientProxy;

class HttpServer {
public:
    enum ErrorCode { OK = 0, CLIENT_ACCEPT_FAIL, SERVER_INITIAL_FAIL};

    HttpServer(int port, bool isCgi = true): port(port), isCgi(isCgi) { }
    ~HttpServer();
    ErrorCode run();

    static Config* config;
private:
    // --- Utilities
    static void close_parent_fd(int fd, short ev, void *arg);
    void run_event_loop(int fd);

    // --- Callbacks
    static void accept_callback(int fd, short ev, void* arg);
    static void client_read_callback(bufferevent *incoming, void *arg);
    static void client_write_callback(bufferevent *incoming, void *arg);
    static void client_error_callback(bufferevent *incoming, short what, void *arg);

    // --- Data
    int port;
    int socket_fd;
    bool isCgi;
};

#endif
