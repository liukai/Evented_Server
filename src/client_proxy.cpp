#include "client_proxy.h"
#include "config.h"
#include "log.h"
#include "util.h"
#include <cassert>
#include <event.h>
#include <iostream>
#include <unistd.h>

using namespace std;

ClientProxy::~ClientProxy() {
    log<<"closing client: "<<client_fd<<endl;

    close(client_fd);
    bufferevent_disable(buf_event, EV_READ | EV_WRITE);
    bufferevent_free(buf_event);
    delete loader;

    if (file_event) {
        event_del(file_event);
        delete file_event;
    }

    log<<"closed "<<client_fd<<endl;
}
void ClientProxy::bad_request(ReturnCode reason) {
    log<<"bad request: "<<reason<<endl;
    http_statue_line(reason, Config::DEFAULT_TYPE);

    const char REASON[] = "Bad Request!";
    int SIZE = (int)sizeof(REASON) - 1;
    http_close(REASON, SIZE);
}

bool ClientProxy::load_resource(const char* resource, const char* query, 
                                const char* type, bool is_static) {
    filetype = type;
    return is_static ? load_static_content(resource) : load_dynamic_content(resource, query);
}

// -- Content Loader
bool ClientProxy::load_static_content(const char* resource) {
    loader = new ContentLoader(resource);
    file_event = new event();
    event_set(file_event, loader->get_fd(),
              EV_READ|EV_PERSIST, file_write_callback,
              this);
    event_add(file_event, NULL);
    return true;
}

// -- HTTP
void ClientProxy::http_data(const char* response, int size) {
    evbuffer *evreturn = evbuffer_new();
    evbuffer_add(evreturn, response, size);
    bufferevent_write_buffer(buf_event, evreturn);
    evbuffer_free(evreturn);
}
void ClientProxy::http_statue_line(ReturnCode code, const char* type) {
    evbuffer *evreturn = evbuffer_new();
    evbuffer_add_printf(evreturn, "HTTP/1.0 %d OK\n", (int)code);
    evbuffer_add_printf(evreturn, "CONTENT-TYPE: %s\n\n", type);
    bufferevent_write_buffer(buf_event, evreturn);
    evbuffer_free(evreturn);
}
void ClientProxy::http_close(const char* response, int size) {
    ready_to_close();
    http_data(response, size); 
}

// -- Callbacks
void ClientProxy::file_write_callback(int disk_fd, short ev, void *arg) {
    log<<"Loading content from disk ... "<<endl;
    ClientProxy* client = (ClientProxy*) arg;
    ContentLoader& loader = *client->loader;

    char buffer[FILE_BUFFER_SIZE];
    if (client->first_time) {
        client->http_statue_line(SUCCESS, client->filetype);
        client->first_time = false;
    }

    cout<<"ready to read:"<<endl;
    int bufferRead = loader.read(buffer, FILE_BUFFER_SIZE);
    if (bufferRead > 0) {
        log<<"\tbuffer: "<<bufferRead<<"/"<<FILE_BUFFER_SIZE<<endl;
        
        client->http_data(buffer, bufferRead);
    } 
    
    if (!loader.can_read()) {
        log<<"\tready to close client: "<<client->client_fd<<endl;
        client->http_close("", 0);
    }
}

