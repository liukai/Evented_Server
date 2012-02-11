#include "client_proxy.h"
#include <event.h>
#include <unistd.h>
#include "util.h"

#include <iostream>
#include <cassert>

using namespace std;
extern int ctrl_pipe[2];

ClientProxy::~ClientProxy() {
    cout<<"ready to delete:"<<client_fd<<endl;
    close(client_fd);
    bufferevent_disable(buf_event, EV_READ | EV_WRITE);
    bufferevent_free(buf_event);
    buf_event = NULL;
    delete loader;
    loader = NULL;
}
void ClientProxy::bad_request(ReturnCode reason) {
    cout<<"bad request"<<endl;
    http_statue_line(reason);

    const char REASON[] = "Bad Request!";
    int SIZE = (int)sizeof(REASON) - 1;
    http_close(REASON, SIZE);
}

bool ClientProxy::load_resource(const char* resource, bool is_static) {
    if (is_static) {
        loader = new ContentLoader(resource);
        event_set(&file_event, loader->get_fd(),
                  EV_READ|EV_PERSIST, file_write_callback,
                  this);
        event_add(&file_event, NULL);
        return true;
    } else {
        return load_dynamic_content(resource);
    }

}

// -- HTTP
void ClientProxy::http_data(const char* response, int size) {
    evbuffer *evreturn = evbuffer_new();
    evbuffer_add(evreturn, response, size);
    bufferevent_write_buffer(buf_event, evreturn);
    evbuffer_free(evreturn);
}
void ClientProxy::http_statue_line(ReturnCode code) {
    evbuffer *evreturn = evbuffer_new();
    evbuffer_add_printf(evreturn, "HTTP/1.0 %d OK\n", (int)code);
    evbuffer_add_printf(evreturn, "CONTENT-TYPE: text/plain\n\n");
    bufferevent_write_buffer(buf_event, evreturn);
    evbuffer_free(evreturn);
}
void ClientProxy::http_close(const char* response, int size) {
    ready_to_close();
    http_data(response, size); 
}

// -- Callbacks
void ClientProxy::file_write_callback(int disk_fd, short ev, void *arg) {
    cout<<"[CALL] file_write_callback"<<endl;
    ClientProxy* client = (ClientProxy*) arg;
    ContentLoader& loader = *client->loader;

    char buffer[FILE_BUFFER_SIZE];
    if (client->first_time) {
        client->http_statue_line(SUCCESS);
        client->first_time = false;
    }

    cout<<"ready to read:"<<endl;
    int bufferRead = loader.read(buffer, FILE_BUFFER_SIZE);
    if (bufferRead > 0) {
        cout<<"buffer read:"<<bufferRead<<endl;
        cout<<"can read: "<<loader.can_read()<<" "<<(int)(bufferRead < 1)<<endl;
        cout<<"buffer: "<<bufferRead<<"\t"<<FILE_BUFFER_SIZE<<endl;
        
        client->http_data(buffer, bufferRead);
    } 
    
    if (!loader.can_read()) {
        cout<<"ready to kill:"<<client->client_fd<<endl;
        event_del(&client->file_event);
        client->http_close("", 0);
    } else {
        cout<<"skip killing"<<endl;
    }
}

bool ClientProxy::load_dynamic_content(const char* resource) {
    int pid = fork();
    cout<<"pid:"<<pid<<endl;
    if (pid == 0) {
        cout<<"parenting"<<endl;
    } else if (pid > 0) {
        dup2(client_fd, STDOUT_FILENO);
        
        ClientProxy* tmp = this;
        cout<<ctrl_pipe[WRITEEND]<<endl;
        write(ctrl_pipe[WRITEEND], tmp, sizeof(ClientProxy*));

        cout<<resource<<endl;
        char* command[] = {(char*)resource, NULL};
        // char *env[] = { "USER=user1", "PATH=/usr/bin:/bin:/opt/bin", (char *) 0 };
        // char *env[] = {NULL};
        if (execve(command[0], command, NULL) != 0) {
            cout<<"exec fails"<<endl;
            exit(1);
        }
    } else {
        return false;
    }
    return true;
}
