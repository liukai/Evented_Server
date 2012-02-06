#ifndef IP_CONTENT_LOADER
#define IP_CONTENT_LOADER

class ContentLoader {
public:
    ContentLoader(int fd): fd(fd), auto_close(false) { }
    ContentLoader(const char* filename);
    ~ContentLoader();

    bool is_valid() const { return fd == -1; }
    void set_auto_close(bool auto_close) { this->auto_close = auto_close; }
    bool is_auto_close() const { return auto_close; }
    int get_fd() const { return fd; }
    bool can_read() const;

    int read(char* buffer, int size);
private:
    int fd;
    bool auto_close;
};

#endif
