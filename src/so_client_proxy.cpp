#include <unistd.h>
#include <dlfcn.h>
#include <stdlib.h>
#include "so_client_proxy.h"
#include "util.h"
#include "log.h"
#include <string>

extern char **environ;
extern int ctrl_pipe[2];
std::map<std::string, Writer> SoClientProxy::libs;

using namespace std;

bool SoClientProxy::load_dynamic_content(const char* resource, const char* query) {
    Writer writer = NULL;
    map<string, Writer>::iterator pos = libs.find(resource);

    if (pos == libs.end()) {
        log<<"Load ["<<resource<<"] for the first time"<<endl;
        void* handler = dlopen(resource, RTLD_LAZY);
        writer = (Writer)dlsym(handler, "write_to");
        libs.insert(make_pair(resource, writer));
    } else {
        writer = pos->second;
    }
    bool ret = writer(dup(client_fd), query);
    ready_to_close();

    return ret;
}
