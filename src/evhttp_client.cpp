#include "evhttp_client.h"
#include <event.h>
#include <unistd.h>

#include <iostream>

using namespace std;

HttpClient::~HttpClient() {
    close(client_fd);
    bufferevent_free(buf_event);
    bufferevent_disable(buf_event, EV_READ | EV_WRITE);
    delete loader;
}
void HttpClient::bad_request(ReturnCode reason) {
    cout<<"bad request"<<endl;
    http_statue_line(reason);

    const char REASON[] = "Bad Request!";
    int SIZE = (int)sizeof(REASON) - 1;
    http_close(REASON, SIZE);
}

void HttpClient::load_resource(const char* resource, bool is_static) {
    if (is_static) {
        loader = new ContentLoader(resource);
    } else {
        cout<<"PIPE"<<endl;
        int pipeline[2];
        pipe(pipeline);

        // TODO: what if fork fails
        if (fork()) {
            close(pipeline[1]);
            loader = new ContentLoader(pipeline[0]);
        } else {
            close(pipeline[0]);
            dup2(pipeline[1], STDOUT_FILENO);

            execlp( "wc", "wc -l", resource, NULL );
        }
    }
    event_set(&file_event, loader->get_fd(),
              EV_READ|EV_PERSIST, file_write_callback,
              this);
    event_add(&file_event, NULL);
}

// -- HTTP
void HttpClient::http_data(const char* response, int size) {
    evbuffer *evreturn = evbuffer_new();
    evbuffer_add(evreturn, response, size);
    bufferevent_write_buffer(buf_event, evreturn);
    evbuffer_free(evreturn);
}
void HttpClient::http_statue_line(ReturnCode code) {
    evbuffer *evreturn = evbuffer_new();
    evbuffer_add_printf(evreturn, "HTTP/1.0 %d OK\n", (int)code);
    evbuffer_add_printf(evreturn, "CONTENT-TYPE: text/plain\n\n");
    bufferevent_write_buffer(buf_event, evreturn);
    evbuffer_free(evreturn);
}
void HttpClient::http_close(const char* response, int size) {
    ready_to_close();
    http_data(response, size); 
}

// -- Callbacks
void HttpClient::file_write_callback(int disk_fd, short ev, void *arg) {
    HttpClient* client = (HttpClient*) arg;
    ContentLoader& loader = *client->loader;

    char buffer[FILE_BUFFER_SIZE];
    if (client->first_time) {
        client->http_statue_line(SUCCESS);
        client->first_time = false;
    }
    int bufferRead = loader.read(buffer, FILE_BUFFER_SIZE);
    cout<<"buffer read:"<<bufferRead<<endl;
    cout<<"can read: "<<loader.can_read();
    cout<<"buffer: "<<bufferRead<<"\t"<<FILE_BUFFER_SIZE<<endl;
    
    if (!loader.can_read()) {    
        cout<<"ready to kill:"<<client->client_fd<<endl;
        client->ready_to_close();
        event_del(&client->file_event);
    }
    client->http_data(buffer, bufferRead);
}

