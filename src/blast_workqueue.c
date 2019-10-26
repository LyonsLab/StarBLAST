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
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/un.h>

#include "socket_comm.h"

#define WQAPP_BACKEND_SOCKET_PATH "/var/www/sequenceserver/backend.server"
#define DEFAULT_JOB_DIR "/tmp"

FILE *log_file;
char *backend_ip = NULL;
int backend_port = -1;
char *backend_sock_path = NULL;
char *job_dir = NULL;

int sock_fd;

int print_file(const char *filename);
int connect_to_backend(const char *serv_ip_addr, int port);
int connect_to_backen_unix(const char *path);
void sigint_handler(int sig);


/*
 * Main
 */
int main(int argc, char *argv[])
{
    int rc;
    char *backend_ip = NULL;
    char *blast_cmd = NULL;

    // Check number of argument
    if(argc == 5)
    {
        job_dir = argv[1];
        backend_ip = argv[2];
        backend_port = atoi(argv[3]);
        blast_cmd = argv[4];
    }
    else if(argc == 4)
    {
        job_dir = argv[1];
        backend_sock_path = argv[2];
        blast_cmd = argv[3];
    }
    else if(argc == 3)
    {
        backend_sock_path = WQAPP_BACKEND_SOCKET_PATH;
        job_dir = argv[1];
        blast_cmd = argv[2];
    }
    else if(argc == 2)
    {
        backend_sock_path = WQAPP_BACKEND_SOCKET_PATH;
        job_dir = DEFAULT_JOB_DIR;
        blast_cmd = argv[1];
    }
    else
    {
        fprintf(stderr, "Default:\n");
        fprintf(stderr, "job-dir: %s\n", DEFAULT_JOB_DIR);
        fprintf(stderr, "unix-socket-path: %s\n\n", WQAPP_BACKEND_SOCKET_PATH);
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "blast_workqueue <cmd-from-sequenceserver>\n");
        fprintf(stderr, "blast_workqueue <job-dir> <cmd-from-sequenceserver>\n");
        fprintf(stderr, "blast_workqueue <job-dir> <backend-unix-sock-path> <cmd-from-sequenceserver>\n");
        fprintf(stderr, "blast_workqueue <job-dir> <backend-ip> <backend-port> <cmd-from-sequenceserver>\n");
        exit(1);
    }

    if(signal(SIGINT, sigint_handler) == SIG_ERR)
    {
        fprintf(stderr, "Cannot catch SIGINT\n");
        return -1;
    }

    if(access(job_dir, R_OK | W_OK | X_OK) < 0)
    {
        fprintf(stderr, "Cannot access job-dir, %s\n", strerror(errno));
        exit(-1);
    }

    // Open the log file
    // stdout reserved for BLAST output, logging has to go through file
    char log_filename[64];
    snprintf(log_filename, 64, "%s/blast-wq-app-%d.log", job_dir, getpid());
    log_file = fopen(log_filename, "w+");
    if(log_file == NULL)
    {
        fprintf(stderr, "Unable to open log file\n");
        exit(1);
    }

    // Connect to the backend
    if(backend_sock_path != NULL)
        sock_fd = connect_to_backen_unix(backend_sock_path);
    else
        sock_fd = connect_to_backend(backend_ip, backend_port);
    if(sock_fd < 0)
    {
        fprintf(stderr, "Unable to connect to backend\n");
        exit(-1);
    }

    // Generate a input filename
    char cmd_file[256];
    rc = snprintf(cmd_file, sizeof(cmd_file), "%s/blast-wq-%d.in", job_dir, getpid());
    if(rc < 0)
    {
        fprintf(stderr, "job_dir too long\n");
        exit(-1);
    }

    // Store the cmd in the input file
    FILE *file = fopen(cmd_file, "w+");
    fwrite(blast_cmd, sizeof(char), strlen(blast_cmd), file);
    fclose(file);

    // Generate a output filename
    char local_outfile[256];
    rc = snprintf(local_outfile, sizeof(local_outfile), "%s/blast-wq-%d.out", job_dir, getpid());
    if(rc < 0)
    {
        fprintf(stderr, "job_dir too long\n");
        exit(-1);
    }

    // Compose msg in the format of <blast-cmd-filename> <output-filename>
    int alloc_len = strlen(local_outfile) + strlen(cmd_file) + 10;
    char *msg = calloc(alloc_len, sizeof(char));
    snprintf(msg, alloc_len, "%s %s", cmd_file, local_outfile);

    // Sent the BLAST cmd to backend
    int byte_sent = write(sock_fd, msg, strlen(msg));
    if(byte_sent <= 0)
    {
        fprintf(stderr, "Unable to send BLAST cmd to backend, %d\n", errno);
        close(sock_fd);
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

int connect_to_backend(const char *serv_ip_addr, int port)
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
    serv_addr.sin_port = htons(port);

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

int connect_to_backen_unix(const char *path)
{
    int clnt_sock, rc;
    struct sockaddr_un serv_addr; 
    memset(&serv_addr, 0, sizeof(struct sockaddr_un));

    clnt_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (clnt_sock == -1)
    {
        fprintf(stderr, "socket() failed, Unable to create socket\n");
        return -1;
    }

    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, path);
    socklen_t serv_addr_size = sizeof(serv_addr);
    rc = connect(clnt_sock, (struct sockaddr *) &serv_addr, serv_addr_size);
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

void sigint_handler(int sig)
{
    fprintf(stdout, "SIGINT, clean up\n");
    close(sock_fd);
    fclose(log_file);
    exit(0);
}