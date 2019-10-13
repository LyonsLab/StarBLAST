#ifndef SOCKET_COMM_H
#define SOCKET_COMM_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/types.h>

int listening_socket_init(const char *ip_addr, const int port);
int listen_connection(int sock);
int accept_connection(int listen_sock);
char *read_from_sock(int sock, int *total_byte_read);
int add_to_epoll(int epfd, int fd);

#endif
