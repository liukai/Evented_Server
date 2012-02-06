#include <iostream>
#include <boost/filesystem.hpp>
#include "socket_util.h"
#include "request.h"
#include "evhttp_server.h"
#include "evhttp_client.h"

using namespace std;
using namespace boost::filesystem;

char HttpServer::CONTENT_DIR[] = "www";

// -- Commands
HttpServer::ErrorCode HttpServer::run() {
    socket_fd = open_socket(port);
    if (socket_fd == -1)
        return SERVER_INITIAL_FAIL;

    run_event_loop(socket_fd);
    cout<<"here"<<endl;
    close(socket_fd);
    return OK;
}

// -- Utilities
void HttpServer::run_event_loop(int fd) {
    event accept_event;

    event_init();
    event_set(&accept_event, fd,
              EV_READ|EV_PERSIST, accept_callback,
              this);
    event_add(&accept_event, NULL);
    event_dispatch();
}

void HttpServer::report_error(HttpServer::ErrorCode error_code) {
    cerr<<"Error: "<<error_code<<endl;
}

// -- Callbacks
void HttpServer::client_read_callback(bufferevent *incoming, void *arg)
{
    HttpClient* client = (HttpClient*) arg;
    char *header = evbuffer_readline(incoming->input);
    
    if (header == NULL) {
        client->bad_request(BAD_REQUEST);
        return;
    }

    path request_path;
    if (!parse_url(header, strlen(header), &request_path)) {
        client->bad_request(BAD_REQUEST);
        return;
    }

    path final_path(CONTENT_DIR);
    final_path /= request_path;

    if (!exists(final_path) || !is_regular_file(final_path)) {
        client->bad_request(NOT_FOUND);
        return;
    }

    cout<<"file: "<<final_path<<endl;
    client->load_resource(final_path.c_str(), false);

    free(header);
}

void client_write_callback(struct bufferevent *bev, void *arg) {
    HttpClient* client = (HttpClient*) arg;
    if (client->can_close()) {
        delete client;
    }
}

void client_error_callback(struct bufferevent *bev, short what, void *arg)
{
    HttpClient *client = (HttpClient *)arg;
    delete client;
}


void HttpServer::accept_callback(int fd, short ev, void *arg)
{
    int client_fd = get_accepted_client(fd);
    cout<<"client fd:"<<client_fd<<endl;

    if (client_fd < 0)
    {
        report_error(CLIENT_ACCEPT_FAIL);
        return;
    }
    set_nonblock(client_fd);

    HttpClient *client = new HttpClient(client_fd);
    bufferevent* buf_ev = bufferevent_new(client_fd,
                                          client_read_callback,
                                          client_write_callback,
                                          client_error_callback,
                                          client);
    client->set_event_buffer(buf_ev);
    bufferevent_enable(buf_ev, EV_READ);
}
