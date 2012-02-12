#include "server.h"
#include "log.h"
#include "config.h"
#include <iostream>
#include <signal.h>

using namespace std;
HttpServer* server = NULL;

void on_interrupt(int sig) {
    log<<"CTRL + C"<<endl;
    if (server != NULL)
        delete server;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        log<<"Please provide the port number:\n\t eva <port> [<config file>] [mode]"<<endl;
        exit(1);
    }

    int port = atoi(argv[1]);
    if (port == 0) {
        log<<"Invalid port number: "<<argv[1]<<endl;
        exit(1);
    }

    signal(SIGINT, on_interrupt);
    const char* config_file = argc > 2 ? argv[2] : "config.ini";
    Config config(config_file);

    bool isCgi = argc < 3 || strcmp(argv[3], "cgi") == 0;
    log<<"Mode: "<<(isCgi ? "CGI" : "SO")<<endl;

    HttpServer::config = &config;
    // TODO: this approach cannot extend easily.
    server = new HttpServer(port, isCgi);
    return server->run();
}
