#include "evhttp_server.h"

int main(int argc, char **argv)
{
    // TODO: the port number
    HttpServer server(8080);
    return server.run();
}
