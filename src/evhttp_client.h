#ifndef IS_EVHTTP_CLIENT_H
#define IS_EVHTTP_CLIENT_H

#include "content_loader.h"
#include <event.h>

enum ReturnCode { SUCCESS = 200, BAD_REQUEST = 400, NOT_FOUND = 404 };
class HttpClient {
public:
    HttpClient(int client_fd): 
               client_fd(client_fd), finished(false), buf_event(NULL),
               first_time(true), loader(NULL) { }
    ~HttpClient();

    void set_event_buffer(bufferevent* buf) { buf_event = buf; }
    void load_resource(const char* resource, bool is_static);
    void bad_request(ReturnCode reason);
    void ready_to_close() { finished = true; }
    bool can_close() const { return finished; }

private:
    // -- Callbacks
    static void file_write_callback(int client_fd, short ev, void *arg);

    // -- HTTP
    void http_statue_line(ReturnCode code); 
    void http_data(const char* response, int size);
    void http_close(const char* response, int size);

    // -- Data
    static const int FILE_BUFFER_SIZE = 1024 * 8;
    int client_fd;
    bool finished;
    bool first_time;
    ContentLoader* loader;
    event file_event;
    bufferevent *buf_event;
};


#endif