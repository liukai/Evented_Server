#include <unistd.h>
#include <stdlib.h>
#include "cgi_client_proxy.h"
#include "util.h"
#include "log.h"

extern char **environ;
extern int ctrl_pipe[2];

using namespace std;

bool CGIClientProxy::load_dynamic_content(const char* resource, const char* query) {
    int pid = fork();
    if (pid > 0) {
        ready_to_close();
        return true;
    }

    if (pid == 0) {
        setenv("QUERY_STRING", query, 1); 
        dup2(client_fd, STDOUT_FILENO);
        
        /*
        void* self = this;
        write(ctrl_pipe[WRITEEND], &self, sizeof(void*));
        */

        char* command[] = {(char*)resource, NULL};
        if (execve(command[0], command, environ) != 0) {
            cout<<"exec fails"<<endl;
            exit(1);
        }
    }
    // implies pid == -1
    return false;
}
