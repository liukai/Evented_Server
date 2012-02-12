#include <unistd.h>
#include <string.h>
#include <string>
#include <pthread.h>
#include <utility>
#include <pthread.h>

using namespace std;

struct data {
    int fd;
    string query;
};
void* write_data(void* arg) {
    int* pfd = (int*)arg;

    char hello[] = "<h1>hello world!</h1>";
    const char header[] = "HTTP/1.0 200 OK\r\n"
                          "CONTENT-TYPE: text/html\r\n\r\n";
    write(*pfd, header, sizeof(header) - 1);    
    for (int i = 0; i < 1000; ++i)
        write(*pfd, hello, sizeof(hello) - 1);
    // write(d->fd, d->query.c_str(), d->query.size());

    close(*pfd);
    delete pfd;
}

extern "C" bool write_to(int fd, const char* query) {
    /*
    data* d = new data;
    d->fd = fd;
    d->query = query;
    */
    pthread_t tid;
    int* pfd = new int(fd);
    pthread_create(&tid, NULL, write_data, pfd);
    return true;
}
