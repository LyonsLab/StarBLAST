#ifndef SOCKET_COMM_H
#define SOCKET_COMM_H



int listening_socket_init(const char *ip_addr, const int port);
int listen_connection(int sock);
int accept_connection(int listen_sock);
char *read_from_sock(int sock, int *total_byte_read);
int add_to_epoll(int epfd, int fd);
int set_fd_nonblock(int fd);

#endif
