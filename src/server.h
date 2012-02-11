#ifndef IS_EVHTTP_SERVER_H
#define IS_EVHTTP_SERVER_H

#include <event.h>
#include "config.h"

class ClientProxy;

class HttpServer {
public:
    enum ErrorCode { OK = 0, CLIENT_ACCEPT_FAIL, SERVER_INITIAL_FAIL};

    HttpServer(int port): port(port) { }
    ErrorCode run();

private:
    // --- Utilities
    static void report_error(ErrorCode error_code);
    static void ready_close_callback(int fd, short ev, void *arg);
    void run_event_loop(int fd);

    // --- Callbacks
    static void accept_callback(int fd, short ev, void* arg);
    static void client_read_callback(bufferevent *incoming, void *arg);
    static void client_write_callback(bufferevent *incoming, void *arg);
    static void client_error_callback(bufferevent *incoming, short what, void *arg);

    // --- Data
    static const char CONTENT_DIR[];
    static Config config;
    int port;
    int socket_fd;
};

#endif

