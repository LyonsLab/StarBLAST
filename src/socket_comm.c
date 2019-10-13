
#include "socket_comm.h"
#include <errno.h>


int listening_socket_init(const char *ip_addr, const int port)
{

    // Create Socket
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Bind Socket with IP addr and Port
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  // IPv4 Addr
    serv_addr.sin_addr.s_addr = inet_addr(ip_addr);
    serv_addr.sin_port = htons(port);
    bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // Set socket to be NONBLOCK
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

    return sock;
}

int listen_connection(int sock)
{
    int rc = 0;

    // Create Epoll
    int epfd = epoll_create(10);
    if(epfd < 0)
    {
        fprintf(stderr, "epoll_create() failed, %d\n", errno);
        exit(-1);
    }

    // Add to epoll
    add_to_epoll(epfd, sock);
    if(rc < 0)
    {
        fprintf(stderr, "epoll_ctl() failed, %d\n", errno);
        exit(-1);
    }

    // Listen, wait for new connection
    printf("Start listen for connection\n");
    rc = listen(sock, 20);
    if(rc < 0)
    {
        fprintf(stderr, "listen() failed, unable to listen on the socket, %d\n", errno);
        exit(-1);
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
        //fprintf(stderr, "accept() failed, unable to accept new connection\n");
        return -1;
    }
    fprintf(stdout, "Connection accepted\n");

    // Set server socket as non-blocking
    int flags = fcntl(conn_sock, F_GETFL, 0);
    if(fcntl(listen_sock, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        fprintf(stderr, "fcntl() failed, unable to set socket as non-blocking\n");
        exit(-1);
    }

    return conn_sock;
}

char *read_from_sock(int sock, int *total_byte_read)
{

    // Read Request
    char buffer[1024];
    char *data = malloc(1024 * sizeof(*data));
    int data_len = 0;

    size_t total_bytes = 0;
    int byte_read = 0;
    do
    {
        byte_read = read(sock, buffer, sizeof(buffer));

        if(byte_read < 0)
        {
            free(data);
            return NULL;
        }

        total_bytes += byte_read;

        memcpy(data + data_len, buffer, byte_read);
        data_len += byte_read;
        if(byte_read > 0)
            data = realloc(data, data_len + 10);
        //printf("read %d byte\n", byte_read);

    }
    while(byte_read == 1024);

    // null-terminate, in case the data is string
    data[total_bytes] = '\0';

    if(total_byte_read != NULL)
        *total_byte_read = total_bytes;

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
        fprintf(stderr, "epoll_ctl() failed, %d\n", errno);
    }
    return rc;
}

#ifdef UNIT_TEST
#include "CuTest.h"

/*
 * Read from pipe
 */
void test1_read_from_sock(CuTest *tc)
{
    int pipefd[2];
    CuAssertTrue(tc, pipe(pipefd) != -1);
    char *msg = "TEST 123";
    write(pipefd[1], msg, strlen(msg)+1);

    int total_byte = -123;
    char *result = read_from_sock(pipefd[0], &total_byte);

    CuAssertPtrNotNull(tc, result);
    CuAssertStrEquals(tc, result, msg);
    CuAssertIntEquals(tc, total_byte, strlen(msg)+1);

    close(pipefd[0]);
    close(pipefd[1]);
    free(result);
}

/*
 * Read from pipe
 */
void test2_read_from_sock(CuTest *tc)
{
    int pipefd[2];
    CuAssertTrue(tc, pipe(pipefd) != -1);
    char *msg = "TEST 123";
    write(pipefd[1], msg, strlen(msg)+1);

    char *result = read_from_sock(pipefd[0], NULL);

    CuAssertPtrNotNull(tc, result);
    CuAssertStrEquals(tc, result, msg);

    close(pipefd[0]);
    close(pipefd[1]);
    free(result);
}

/*
 * Read from write end of pipe
 */
void test3_read_from_sock(CuTest *tc)
{
    int pipefd[2];
    CuAssertTrue(tc, pipe(pipefd) != -1);

    char *result = read_from_sock(pipefd[1], NULL);

    CuAssertPtrEquals(tc, result, NULL);

    close(pipefd[0]);
    close(pipefd[1]);
}

/*
 * Read from empty pipe, NONBLOCK
 */
void test4_read_from_sock(CuTest *tc)
{
    int pipefd[2];
    CuAssertTrue(tc, pipe(pipefd) != -1);

    int flags = fcntl(pipefd[0], F_GETFL, 0);
    CuAssertTrue(tc, flags != -1);
    CuAssertTrue(tc, fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK) != -1);

    char *result = read_from_sock(pipefd[0], NULL);

    CuAssertPtrEquals(tc, result, NULL);

    close(pipefd[0]);
    close(pipefd[1]);
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

#endif
