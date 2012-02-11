#include "server.h"
#include "log.h"

int main(int argc, char **argv)
{
    init_debugger();
    HttpServer server(8080);
    return server.run();
}
