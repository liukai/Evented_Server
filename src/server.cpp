#include "gateway.h"

int main(int argc, char** argv) {
    int port = atoi(argv[1]);
    Gateway gateway(port);
    gateway.run();

    return 0;
}
