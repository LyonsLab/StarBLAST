/*
 * A "dummy" frontend for WorkQueue backend.
 * 
 * BLAST command are sent to backend for scheduling and dispatching.
 * The content of the temp output file is printed to stdout for SeqServer
 * to capture.
 * 
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define WQAPP_BACKEND_PORT 1999

FILE *log_file;


int print_file(const char *filename);
int connect_to_backend(const char *serv_ip_addr);
char *read_from_socket(int sock);


/*
 * Main
 */
int main(int argc, char *argv[])
{
    int rc;
    char *backend_ip = NULL;
    char *blast_cmd = NULL;

    // Check number of argument
    if(argc == 3)
    {
        backend_ip = argv[1];
        blast_cmd = argv[2];
    }
    else if(argc == 2)
    {
        backend_ip = "127.0.0.1";
        blast_cmd = argv[1];
    }
    else
    {
        fprintf(stderr, "blast_workqueue <cmd-from-sequenceserver>\n");
        fprintf(stderr, "blast_workqueue <backend-ip> <cmd-from-sequenceserver>\n");
        exit(1);
    }

    // Open the log file
    // stdout reserved for BLAST output, logging has to go through file
    char log_filename[64];
    snprintf(log_filename, 64, "/tmp/blast-wq-app-%d.log", getpid());
    log_file = fopen(log_filename, "w+");
    if(log_file == NULL)
    {
        fprintf(stderr, "Unable to open log file\n");
        exit(1);
    }

    // Connect to the backend
    int sock_fd = connect_to_backend(backend_ip);
    if(sock_fd < 0)
    {
        fprintf(stderr, "Unable to connect to backend\n");
        exit(-1);
    }

    // Generate a input filename
    char cmd_file[100];
    snprintf(cmd_file, 100, "/tmp/blast-wq-%d.in", getpid());

    // Store the cmd in the input file
    FILE *file = fopen(cmd_file, "w+");
    fwrite(blast_cmd, sizeof(char), strlen(blast_cmd), file);
    fclose(file);

    // Generate a output filename
    char local_outfile[100];
    snprintf(local_outfile, 100, "/tmp/blast-wq-%d.out", getpid());

    // Compose msg in the format of <blast-cmd-filename> <output-filename>
    int alloc_len = strlen(local_outfile) + strlen(cmd_file) + 10;
    char *msg = calloc(alloc_len, sizeof(char));
    snprintf(msg, alloc_len, "%s %s", cmd_file, local_outfile);

    // Sent the BLAST cmd to backend
    int byte_sent = write(sock_fd, msg, strlen(msg));
    if(byte_sent <= 0)
    {
        fprintf(stderr, "Unable to send BLAST cmd to backend, %d\n", errno);
        exit(-1);
    }

    // Wait until the result come back
    while(access(local_outfile, R_OK) != 0) sleep(1);

    // Print the output file to stdout, in order to mimc the behavior of
    // blast invocation by SeqServer
    rc = print_file(local_outfile);
    if(rc < 0)
    {
        fprintf(stderr, "Unable to read output file\n");
        exit(-1);
    }

    fprintf(log_file, "tasks complete!\n");

    close(sock_fd);
    fclose(log_file);

    return 0;
}

int connect_to_backend(const char *serv_ip_addr)
{
    int rc;

    int clnt_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clnt_sock < 0)
    {
        fprintf(stderr, "socket() failed, Unable to create socket\n");
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(WQAPP_BACKEND_PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form 
    rc = inet_pton(AF_INET, serv_ip_addr, &serv_addr.sin_addr);
    if(!rc)
    {
        fprintf(stderr, "inet_pton failed(), Unable to convert ip address, return code %d\n", rc);
        return -1;
    }

    // Connect to backend
    socklen_t serv_addr_size = sizeof(serv_addr);
    rc = connect(clnt_sock, (struct sockaddr*)&serv_addr, serv_addr_size);
    if (rc < 0)
    {
        fprintf(stderr, "connect() failed, Unable to initiate a connection on socket, return code %d, errno %d\n", rc, errno);
        return -1;
    }

    return clnt_sock;
}

/*
 * Print the content of a file
 * 
 * \param filename Name of the file to be printed to stdout
 * \return Return 0 if success, -1 otherwise
 */
int print_file(const char *filename)
{
    char c;

    if(filename == NULL) return -1;

    FILE *file = fopen(filename, "r");
    if (file)
    {
        while ((c = getc(file)) != EOF)
            fprintf(stdout, "%c", c);
        fclose(file);
        return 0;
    }

    return -1;
}

char *read_from_socket(int sock)
{

    // Read Request
    char buffer[1024];
    char *request_str = malloc(1024 * sizeof(*request_str));
    int request_str_len = 0;

    int byte_read = 0;
    do
    {
        byte_read = read(sock, buffer, sizeof(buffer));

        if(byte_read < 0) return NULL;

        memcpy(request_str + request_str_len, buffer, byte_read);
        request_str_len += byte_read;
        request_str = realloc(request_str, request_str_len);
        fprintf(log_file, "read %d byte\n", byte_read);

    }
    while(byte_read == 1024);

    fprintf(log_file, "\"%s\"\n", request_str);

    return request_str;
}


