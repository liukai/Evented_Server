#ifndef IS_EVHTTP_CLIENT_H
#define IS_EVHTTP_CLIENT_H

#include "content_loader.h"
#include <event.h>

enum ReturnCode { SUCCESS = 200, BAD_REQUEST = 400, NOT_FOUND = 404,
                  REQUEST_DENIED = 500 };

class ClientProxy {
public:
    ClientProxy(int client_fd): 
               client_fd(client_fd), finished(false),
               first_time(true), loader(NULL), file_event(NULL) { }
    virtual ~ClientProxy();

    void set_event_buffer(bufferevent* buf) { buf_event = buf; }
    bool load_resource(const char* resource, const char* query, 
                       const char* type, bool is_static);
    void bad_request(ReturnCode reason);
    void ready_to_close() { finished = true; }
    bool can_close() const { return finished; }

    virtual const char* get_directory() const = 0;

    // -- HTTP
    void http_statue_line(ReturnCode code, const char* type); 
    void http_data(const char* response, int size);
    void http_close(const char* response, int size);
protected:
    bool load_static_content(const char*  resource);
    virtual bool load_dynamic_content(const char*  resource, const char* query) = 0;

    int client_fd;
    const char* filetype;

private:
    // -- Callbacks
    static void file_write_callback(int client_fd, short ev, void *arg);

    // -- Data
    static const int FILE_BUFFER_SIZE = 1024 * 8;
    bool finished;
    bool first_time;
    ContentLoader* loader;
    event* file_event;
    bufferevent *buf_event;
};


#endif
