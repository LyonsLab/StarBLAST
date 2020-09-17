
#include "socket_comm.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/un.h>


int listening_socket_init(const char *ip_addr, const int port)
{
    int rc;
    // Create Socket
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock < 0)
    {
        fprintf(stderr, "socket() failed to create socket, %s\n", strerror(errno));
        return -1;
    }

    // SO_REUSEADDR
    int enable = 1;
    rc = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    if(rc < 0)
    {
        fprintf(stderr, "setsockopt() failed to set SO_REUSEADDR, %s\n", strerror(errno));
        close(sock);
        return -1;
    }

    // Bind Socket with IP addr and Port
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  // IPv4 Addr
    serv_addr.sin_addr.s_addr = inet_addr(ip_addr);
    serv_addr.sin_port = htons(port);
    rc = bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(rc < 0)
    {
        fprintf(stderr, "bind() failed to bind the socket to the addr and port, %s\n", strerror(errno));
        close(sock);
        return -1;
    }

    // Set socket to be NONBLOCK
    rc = set_fd_nonblock(sock);
    if(rc < 0)
    {
        close(sock);
        return -1;
    }

    return sock;
}

int listening_unix_socket_init(const char *pathname)
{
    int rc;
    struct sockaddr_un serv_addr;

    if(pathname == NULL) return -1;
    if(strlen(pathname) >= sizeof(serv_addr.sun_path)) return -1;

    // Create socket
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1)
    {
        fprintf(stderr, "socket() failed to create socket, %s\n", strerror(errno));
        return -1;
    }
    
    // Config path
    serv_addr.sun_family = AF_UNIX;
    // strncpy not needed b/c of previous check
    strcpy(serv_addr.sun_path, pathname);
    int len = sizeof(serv_addr);
    
    // Unlink the path and bind
    unlink(pathname);
    rc = bind(sock, (struct sockaddr *) &serv_addr, len);
    if (rc == -1)
    {
        fprintf(stderr, "bind() failed to bind the socket to path %s, %s\n", pathname, strerror(errno));
        close(sock);
        return -1;
    }

    // Set socket to be NONBLOCK
    rc = set_fd_nonblock(sock);
    if(rc < 0)
    {
        close(sock);
        return -1;
    }

    return sock;
}

int listen_connection(int sock)
{
    int rc = 0;

    // Create Epoll
    int epfd = epoll_create(10);
    if(epfd < 0)
    {
        fprintf(stderr, "epoll_create() failed, %s\n", strerror(errno));
        return -1;
    }

    // Add to epoll
    rc = add_to_epoll(epfd, sock);
    if(rc < 0)
    {
        close(epfd);
        return -1;
    }

    // Listen, wait for new connection
    rc = listen(sock, 20);
    if(rc < 0)
    {
        close(epfd);
        fprintf(stderr, "listen() failed to listen on the socket, %s\n", strerror(errno));
        return -1;
    }

    return epfd;
}

int accept_connection(int listen_sock)
{
    // Accept
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int conn_sock = accept(listen_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if(conn_sock < 0)
    {
        fprintf(stderr, "accept() failed to accept new connection, %s\n", strerror(errno));
        return -1;
    }

    // Set connection socket to be non-blocking
    int rc = set_fd_nonblock(conn_sock);
    if(rc < 0)
    {
        close(conn_sock);
        return -1;
    }

    return conn_sock;
}

char *read_from_sock(int sock, int *total_byte_read)
{
    // Read Request
    char buffer[1024];
    char *data = NULL;

    size_t total_alloc = 1;     // save for '\0'
    size_t data_len = 0;
    int byte_read = 0;
    do
    {
        byte_read = recv(sock, buffer, sizeof(buffer), 0);

        if(byte_read < 0)
        {
            if(data != NULL)
                free(data);
            return NULL;
        }
        else if(byte_read == 0)
            break;

        total_alloc += byte_read;

        // First alloc
        if(data == NULL)
            data = malloc(total_alloc * sizeof(*data));
        else
        {
            char *new_alloc = realloc(data, total_alloc * sizeof(*data));
            if(new_alloc == NULL)
            {
                free(data);
                return NULL;
            }
            data = new_alloc;
        }

        memcpy(data + data_len, buffer, byte_read);
        data_len += byte_read;

    }
    while(byte_read == 1024);

    // null-terminate, in case the data is string and not null-term
    if(data != NULL)
        data[data_len] = '\0';

    if(total_byte_read != NULL)
        *total_byte_read = data_len;

    return data;
}

int add_to_epoll(int epfd, int fd)
{
    // Add to epoll
    static struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    int rc = epoll_ctl(epfd, EPOLL_CTL_ADD, ev.data.fd, &ev);
    if(rc < 0)
    {
        fprintf(stderr, "epoll_ctl() failed, %s\n", strerror(errno));
    }
    return rc;
}

int set_fd_nonblock(int fd)
{
    // Get the status flag of fd
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags < 0)
    {
        fprintf(stderr, "fcntl() failed to retrieve flags, %s\n", strerror(errno));
        return -1;
    }

    // Add O_NONBLOCK to the status flag
    int rc = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if(rc < 0)
    {
        fprintf(stderr, "fcntl() failed to set socket with O_NONBLOCK, %s\n", strerror(errno));
        return -1;
    }

    return rc;
}

#ifdef UNIT_TEST
#include "CuTest.h"

char socket_path[100] = "test.sock";
int sock_helper_start(int *serv, int *clnt);
int sock_helper_end(int serv, int clnt);

/*
 * Read from socket
 */
void test1_read_from_sock(CuTest *tc)
{
    int serv_sock;
    int conn_sock;
    int rc = sock_helper_start(&serv_sock, &conn_sock);
    CuAssertTrue(tc, rc == 0);
    char msg[] = "TEST 123";
    rc = send(serv_sock, msg, strlen(msg)+1, 0);
    CuAssertTrue(tc, rc == (int)strlen(msg)+1);

    int total_byte = -123;
    char *result = read_from_sock(conn_sock, &total_byte);

    CuAssertPtrNotNull(tc, result);
    CuAssertStrEquals(tc, result, msg);
    CuAssertIntEquals(tc, strlen(msg)+1, total_byte);

    free(result);
    sock_helper_end(serv_sock, conn_sock);
}

/*
 * Read from socket
 */
void test2_read_from_sock(CuTest *tc)
{
    int serv_sock;
    int conn_sock;
    int rc = sock_helper_start(&serv_sock, &conn_sock);
    CuAssertTrue(tc, rc == 0);
    char *msg = "TEST 123";
    rc = send(serv_sock, msg, strlen(msg)+1, 0);
    CuAssertTrue(tc, rc == (int)strlen(msg)+1);

    char *result = read_from_sock(conn_sock, NULL);

    CuAssertPtrNotNull(tc, result);
    CuAssertStrEquals(tc, result, msg);

    free(result);
    sock_helper_end(serv_sock, conn_sock);
}

/*
 * Read from socket, long msg
 */
void test3_read_from_sock(CuTest *tc)
{
    int serv_sock;
    int conn_sock;
    int rc = sock_helper_start(&serv_sock, &conn_sock);
    CuAssertTrue(tc, rc == 0);

    const size_t msg_len = 2200;
    char *msg = calloc(msg_len + 1, sizeof(char));
    for(size_t i = 0; i < msg_len; i++)
        msg[i] = 'a' + i % 10;
    msg[msg_len] = '\0';
    rc = send(serv_sock, msg, strlen(msg)+1, 0);
    CuAssertTrue(tc, rc == (int)strlen(msg)+1);

    char *result = read_from_sock(conn_sock, NULL);

    CuAssertPtrNotNull(tc, result);
    CuAssertStrEquals(tc, result, msg);

    free(result);
    free(msg);
    sock_helper_end(serv_sock, conn_sock);

}

/*
 * Read from closed sock
 */
void test4_read_from_sock(CuTest *tc)
{
    int serv_sock;
    int conn_sock;
    int rc = sock_helper_start(&serv_sock, &conn_sock);
    CuAssertTrue(tc, rc == 0);

    close(conn_sock);

    char *result = read_from_sock(conn_sock, NULL);

    CuAssertPtrEquals(tc, result, NULL);

    sock_helper_end(serv_sock, conn_sock);
}

/*
 * Read from empty sock, NONBLOCK
 */
void test5_read_from_sock(CuTest *tc)
{
    int serv_sock;
    int conn_sock;
    int rc = sock_helper_start(&serv_sock, &conn_sock);
    CuAssertTrue(tc, rc == 0);

    int flags = fcntl(conn_sock, F_GETFL, 0);
    CuAssertTrue(tc, flags != -1);
    CuAssertTrue(tc, fcntl(conn_sock, F_SETFL, flags | O_NONBLOCK) != -1);

    char *result = read_from_sock(conn_sock, NULL);

    CuAssertPtrEquals(tc, result, NULL);

    sock_helper_end(serv_sock, conn_sock);

}

CuSuite* socket_comm_suite()
{
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, test1_read_from_sock);
    SUITE_ADD_TEST(suite, test2_read_from_sock);
    SUITE_ADD_TEST(suite, test3_read_from_sock);
    SUITE_ADD_TEST(suite, test4_read_from_sock);

    return suite;
}

int sock_helper_start(int *serv, int *clnt)
{
    int rc;
    struct sockaddr_un serv_addr;
    socklen_t serv_addr_size = sizeof(serv_addr);

    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, socket_path);

    unlink(socket_path);
    int listen_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if(listen_sock < 0) fprintf(stderr, "Cannot setup unix socket for this test\n");
    rc = bind(listen_sock, (struct sockaddr *) &serv_addr, serv_addr_size);
    if(rc < 0) fprintf(stderr, "Cannot setup unix socket for this test\n");
    rc = listen(listen_sock, 20);
    if(rc < 0) fprintf(stderr, "Cannot setup unix socket for this test\n");

    int clnt_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if(clnt_sock < 0) fprintf(stderr, "Cannot setup unix socket for this test\n");
    rc = connect(clnt_sock, (struct sockaddr *) &serv_addr, serv_addr_size);
    if(rc < 0) fprintf(stderr, "Cannot setup unix socket for this test\n");

    struct sockaddr_in clnt_addr;
    int conn_sock = accept(listen_sock, (struct sockaddr*)&clnt_addr, &serv_addr_size);

    *serv = conn_sock;
    *clnt = clnt_sock;

    return 0;
}

int sock_helper_end(int serv, int clnt)
{
    close(serv);
    close(clnt);
    unlink(socket_path);
    return 0;
}

#endif
