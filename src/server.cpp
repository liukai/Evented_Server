#include <iostream>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>
#include "util.h"
#include "request.h"
#include "server.h"
#include "log.h"
#include "cgi_client_proxy.h"
#include "so_client_proxy.h"

using namespace std;
using namespace boost::filesystem;

// -- Variables
// TODO ugly
int ctrl_pipe[2];
Config* HttpServer::config = NULL;

// -- Public Interface
HttpServer::~HttpServer() {
    log<<"Logging out..."<<endl;
    event_loopexit(NULL);
    close(socket_fd);
}
HttpServer::ErrorCode HttpServer::run() {
    socket_fd = open_socket(port);
    if (socket_fd == -1) {
        log<<"open socket fails"<<endl;
        exit(1);
    }
    run_event_loop(socket_fd);
    close(socket_fd);

    return OK;
}
void HttpServer::run_event_loop(int fd) {
    // Initial the accept event
    event accept_event;
    event_init();
    event_set(&accept_event, fd,
              EV_READ|EV_PERSIST, accept_callback,
              this);
    event_add(&accept_event, NULL);

    // Initialize the pipe
    event ctrl_event;
    pipe(ctrl_pipe);
    set_nonblock(ctrl_pipe[READEND]);
    set_nonblock(ctrl_pipe[WRITEEND]);
    event_set(&ctrl_event, ctrl_pipe[READEND],
              EV_READ|EV_PERSIST, close_parent_fd,
              this);
    event_add(&ctrl_event, NULL);

    // Start the engine
    event_dispatch();
}

// -- Callbacks
void HttpServer::accept_callback(int fd, short ev, void *arg)
{
    HttpServer* server = (HttpServer*)arg;
    int client_fd = get_accepted_client(fd);
    log<<"accepting new connection: "<<fd<<endl;

    if (client_fd < 0)
    {
        log<<"Cannot get the access the client"<<endl;
        return;
    }

    set_nonblock(client_fd);
    ClientProxy *client = server->isCgi ? (ClientProxy*)new CGIClientProxy(client_fd) 
                                        : (ClientProxy*)new SoClientProxy(client_fd);
    bufferevent* event_buffer = bufferevent_new(client_fd,
                                                client_read_callback,
                                                client_write_callback,
                                                client_error_callback,
                                                client);

    client->set_event_buffer(event_buffer);
    bufferevent_enable(event_buffer, EV_READ);
}

void HttpServer::client_read_callback(bufferevent *incoming, void *arg)
{
    ClientProxy* client = (ClientProxy*) arg;
    char *header = evbuffer_readline(incoming->input);
    
    if (header == NULL) {
        free(header);
        client->bad_request(BAD_REQUEST);
        return;
    }

    path request_path;
    string query;
    if (!parse_url(header, strlen(header), &request_path, &query)) {
        free(header);
        client->bad_request(BAD_REQUEST);
        return;
    }
    log<<"PATH: "<<request_path<<endl;
    log<<"QUERY: \""<<query<<"\""<<endl;

    path final_path(client->get_directory());
    final_path /= request_path;
    log<<"RESOURCE: "<<final_path<<endl;

    if (!exists(final_path) || !is_regular_file(final_path)) {
        free(header);
        client->bad_request(NOT_FOUND);
        return;
    }

    const char* path_str = final_path.c_str();
    size_t size = strlen(path_str);
    string ext;
    ResourceType type = config->get_file_type(path_str, path_str + size, ext);

    if (type == INVALID) {
        client->bad_request(REQUEST_DENIED);
        return;
    }

    const char* return_type = config->get_result_type(ext.c_str());
    if (!client->load_resource(final_path.c_str(), query.c_str(),
                               return_type, type == STATIC)) {
        client->bad_request(REQUEST_DENIED);
    }
    if (client->can_close()) {
        log<<"Delete client at read"<<endl;
        delete client;
    }
    free(header);
}

// for dynamic content
void HttpServer::close_parent_fd(int fd, short ev, void *arg) {
    ClientProxy* client = NULL;
    int byteRead = read(fd, &client, sizeof(ClientProxy*));
    if (byteRead) {
        log<<"client: "<<client<<endl;
        delete client;
    }
    err<<"Cannot read the content from: "<<fd<<endl;
}

void HttpServer::client_error_callback(struct bufferevent *bev, short what, void *arg)
{
    ClientProxy *client = (ClientProxy *)arg;
    delete client;
}

void HttpServer::client_write_callback(struct bufferevent *bev, void *arg) {
    log<<"[CALL] Inside client write callback"<<endl;
    ClientProxy* proxy = (ClientProxy*) arg;
    if (proxy->can_close()) {
        delete proxy;
    }
}

