#include <fcntl.h>
#include <unistd.h>
#include "content_loader.h"

ContentLoader::ContentLoader(const char* filename): auto_close(true) {
    // What if error occurs
    fd = open(filename, O_RDONLY);
}

ContentLoader::~ContentLoader() {
    if (auto_close && is_valid())
        close(fd);
}
bool ContentLoader::can_read() const {
    return fcntl(fd, F_SETFL) != -1;
}

int ContentLoader::read(char* buffer, int size) {
    int byteRead = ::read(fd, buffer, size - 1);
    buffer[byteRead] = '\0';
    if (byteRead < size - 1)
        close(fd);
    return byteRead;
}
