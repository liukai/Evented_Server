#include <iostream>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>
#include "util.h"
#include "request.h"
#include "server.h"
#include <zmq.h>
#include "log.h"
#include "client_proxy.h"

using namespace std;
using namespace boost::filesystem;

// -- Variables
// TODO ugly
int ctrl_pipe[2];
const char HttpServer::CONTENT_DIR[] = "www";
Config HttpServer::config("config.ini");

// -- Initialization
HttpServer::ErrorCode HttpServer::run() {
    int socket_fd = open_socket(port);
    if (socket_fd == -1) {
        exit(1);
    }
    run_event_loop(socket_fd);
    close(socket_fd);

    return OK;
}

void HttpServer::run_event_loop(int fd) {
    event accept_event;
    event ctrl_event;

    event_init();
    event_set(&accept_event, fd,
              EV_READ|EV_PERSIST, accept_callback,
              this);

    pipe(ctrl_pipe);

    set_nonblock(ctrl_pipe[READEND]);
    set_nonblock(ctrl_pipe[WRITEEND]);
    event_set(&ctrl_event, ctrl_pipe[READEND],
              EV_READ|EV_PERSIST, ready_close_callback,
              this);
    event_add(&ctrl_event, NULL);

    event_add(&accept_event, NULL);
    event_dispatch();
}


// -- Callbacks
void HttpServer::accept_callback(int fd, short ev, void *arg)
{
    int client_fd = get_accepted_client(fd);
    log<<"accepting new connection: "<<fd<<endl;

    if (client_fd < 0)
    {
        report_error(CLIENT_ACCEPT_FAIL);
        return;
    }
    set_nonblock(client_fd);

    ClientProxy *client = new ClientProxy(client_fd);
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
    if (!parse_url(header, strlen(header), &request_path)) {
        free(header);
        client->bad_request(BAD_REQUEST);
        return;
    }

    path final_path(CONTENT_DIR);
    final_path /= request_path;

    if (!exists(final_path) || !is_regular_file(final_path)) {
        free(header);
        client->bad_request(NOT_FOUND);
        return;
    }

    const char* path_str = final_path.c_str();
    size_t size = strlen(path_str);
    ResourceType type = config.get_file_type(path_str, path_str + size);

    if (type == INVALID) {
        client->bad_request(REQUEST_DENIED);
        return;
    }

    if (!client->load_resource(final_path.c_str(), type == STATIC)) {
        client->bad_request(REQUEST_DENIED);
    }
    /*
        int pid = fork();
        cout<<"pid: "<<pid<<endl;
        if (pid) {
            cout<<"parenting"<<endl;
        } else {
            dup2(client->client_fd, STDOUT_FILENO);
            write(ctrl_pipe[WRITEEND], &client, sizeof(ClientProxy*));

            char* command[] = {(char*)final_path.c_str(), NULL};
            // char *env[] = { "USER=user1", "PATH=/usr/bin:/bin:/opt/bin", (char *) 0 };
            // char *env[] = {NULL};
            if (execve(command[0], command, NULL) != 0) {
                cout<<"exec fails"<<endl;
                exit(1);
            }
        }
        */
    free(header);
}

// TODO
void HttpServer::ready_close_callback(int fd, short ev, void *arg) {
    ClientProxy* client = NULL;
    int byteRead = read(fd, &client, sizeof(ClientProxy*));
    cout<<"client: "<<client<<endl;
    delete client;
}

// TODO
void HttpServer::client_error_callback(struct bufferevent *bev, short what, void *arg)
{
    /*
    ClientProxy *client = (ClientProxy *)arg;
    delete client;
    */
}



void HttpServer::client_write_callback(struct bufferevent *bev, void *arg) {
    cout<<"call this crap"<<endl;
    // TODO TEST
    ClientProxy* client = (ClientProxy*) arg;
    if (client->can_close()) {
        delete client;
    }
}

// -- Utilities
void HttpServer::report_error(HttpServer::ErrorCode error_code) {
    cerr<<"Error: "<<error_code<<endl;
}

