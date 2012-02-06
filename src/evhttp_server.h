#ifndef IS_EVHTTP_SERVER_H
#define IS_EVHTTP_SERVER_H

#include <event.h>

class HttpClient;

class HttpServer {
public:
    enum ErrorCode { OK=0, CLIENT_ACCEPT_FAIL, SERVER_INITIAL_FAIL};

    HttpServer(int port): port(port) { }
    ErrorCode run();

private:
    // --- Utilities
    static void report_error(ErrorCode error_code);
    void run_event_loop(int fd);

    // --- Callbacks
    static void accept_callback(int fd, short ev, void* arg);
    static void client_read_callback(bufferevent *incoming, void *arg);

    // --- Data
    static char CONTENT_DIR[];
    int port;
    int socket_fd;
};

#endif

