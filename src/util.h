#ifndef IP_SOCKET_UTIL
#define IP_SOCKET_UTIL

int open_socket(int port);
int set_nonblock(int fd);
int get_accepted_client(int listen_fd);
const int READEND = 0;
const int WRITEEND = 1;

#endif
