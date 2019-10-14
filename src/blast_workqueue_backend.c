/*
 * Backend
 */
#include "work_queue.h"
#include "socket_comm.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define WQAPP_BACKEND_PORT 1999
FILE *log_file;

int start_recv_jobs(struct work_queue *q);
int handle_network_activity(int epfd, int listen_sock, struct work_queue *q);
int handle_msg(char *msg, struct work_queue *q);
int parse_msg(const char *msg, char **local_infile, char **local_outfile, char **cmd);
char *parse_infile_from_cmd(const char *cmd, int *len);
int augument_cmd(char **cmd, char *local_infile, char *remote_outfile);
int create_task(struct work_queue *q, char *cmd_str, char *local_infile, char *local_outfile);
int wait_result(struct work_queue *q);

int main(int argc, char *argv[])
{
    struct work_queue *q;
    int port = WORK_QUEUE_DEFAULT_PORT;
    int rc = 0;

    // Open the log file
    // stdout reserved for BLAST output, logging has to go through file
    log_file = fopen("/tmp/blast-wq-backend-app.log", "w+");
    if(log_file == NULL)
    {
        fprintf(stderr, "Unable to open log file\n");
        exit(1);
    }

    // Create job queue
    q = work_queue_create(port);
    if(!q)
    {
        fprintf(stderr, "couldn't listen on port %d: %s\n", port, strerror(errno));
        return 1;
    }
    fprintf(stdout, "WorkQueue listening on port %d...\n", work_queue_port(q));


    // Main Loop
    rc = start_recv_jobs(q);
    if(rc < 0)
    {
        exit(-1);
    }

    fclose(log_file);

    return rc;
}

int start_recv_jobs(struct work_queue *q)
{
    const char ip_addr[] = "127.0.0.1";
    int rc = 0;

    // Create Socket
    int listen_sock = listening_socket_init(ip_addr, WQAPP_BACKEND_PORT);

    // Listen, and Create EPOLL
    int epfd = listen_connection(listen_sock);
    fprintf(stdout, "listening on port %d...\n", WQAPP_BACKEND_PORT);

    while(1)
    {
        rc = handle_network_activity(epfd, listen_sock, q);
        if(rc < 0) continue;

        // Check for result & Send task to worker
        rc = wait_result(q);
        if(rc < 0) continue;
    }

    return rc;
}

int handle_network_activity(int epfd, int listen_sock, struct work_queue *q)
{
    int rc = 0;
    struct epoll_event events[5];

    // check for epoll events
    int nfds = epoll_wait(epfd, events, 5, 1);
    for(int i = 0; i < nfds; i++)
    {
        // New Conn
        if(events[i].data.fd == listen_sock)
        {
            int conn_sock = accept_connection(listen_sock);
            if(conn_sock < 0) continue;

            rc = add_to_epoll(epfd, conn_sock);
            if(rc < 0) close(conn_sock);
        }
        // Incoming Data
        else
        {
            int total_bytes = 0;
            char *msg = read_from_sock(events[i].data.fd, &total_bytes);

            if(msg != NULL && total_bytes > 0)
            {
                handle_msg(msg, q);
                free(msg);
            }
            // Close Connection
            close(events[i].data.fd);
            
        }
    }

    return rc;
}

int handle_msg(char *msg, struct work_queue *q)
{
    int rc = 0;

    fprintf(stdout, "msg: %s\n", msg);
    char *local_outfile = NULL;
    char *local_infile = NULL;
    char *cmd = NULL;

    rc = parse_msg(msg, &local_infile, &local_outfile, &cmd);
    rc = augument_cmd(&cmd, local_infile, basename(local_outfile));

    //fprintf(stdout, "creating task\nin:%s\nout:%s\n", local_infile, local_outfile);
    fprintf(stdout, "creating task\n");

    // Create and submit task
    rc = create_task(q, cmd, local_infile, local_outfile);

    free(local_infile);
    free(local_outfile);
    free(cmd);

    return rc;
}

/*
 * msg is in the format of "<cmd_file> <outfile>"
 */
int parse_msg(const char *msg, char **local_infile, char **local_outfile, char **cmd)
{
    int rc = 0;
    char *fst_space = strchr(msg, ' ');
    if(fst_space == NULL) return -1;

    // Parse out filename of outfile
    char *outfile = fst_space+1;

    *local_outfile = calloc(strlen(fst_space) + 10, sizeof(char));
    strncpy(*local_outfile, outfile, strlen(fst_space));

    // Read cmd
    char cmd_file[fst_space - msg + 10];
    strncpy(cmd_file, msg, fst_space - msg);
    cmd_file[fst_space - msg] = '\0';

    FILE *f = fopen(cmd_file, "r");
    if(f == NULL)
    {
        fprintf(stderr, "Unable to open cmdfile\n");
        return -1;
    }
    else
    {
        char *buffer = calloc(1024, sizeof(char));
        int byte_read = fread(buffer, sizeof(char), 1024, f);
        if(byte_read <= 0)
        {
            fprintf(stderr, "Unable to read anything from cmdfile, read %d bytes\n", byte_read);
        }
        *cmd = calloc(strlen(buffer) + 10, sizeof(char));
        strcpy(*cmd, buffer);

        // Read out filename of input file
        int infilename_len = 0;
        char *infile = parse_infile_from_cmd(*cmd, &infilename_len);
        *local_infile = calloc(infilename_len + 10, sizeof(char));
        strncpy(*local_infile, infile, infilename_len);
        (*local_infile)[infilename_len] = '\0';

        free(buffer);
    }
    fclose(f);

    return rc;
}

/*
 * e.g. blastn -db '/db/est_human /db/vector' -query '12345.fa' -evalue 1e-5
 */
char *parse_infile_from_cmd(const char *cmd, int *len)
{
    if(cmd == NULL || len == NULL) return NULL;

    char *ptr = NULL;
    *len = 0;

    ptr = strstr(cmd, "-query");
    if(ptr == NULL) return NULL;

    char *start;
    if((ptr[6] == ' ' || ptr[6] == '=') && ptr[7] != '\0')
        start = ptr + 7;
    else
        return NULL;        
    char *end = strchr(start + 1, ' ');
    if(end == NULL)
        *len = strlen(cmd) - (start - cmd);
    else
        *len = end - start;


    // if qfile is in single quote
    if(*start == '\'' && *(end-1) == '\'')
    {
        start++;
        *len -= 2;
    }
    return start;
}


/*
 * e.g.
 * from:
 * blastn -db '/db/est_human /db/vector' -query '/var/www/12345.fa' -evalue 1e-5
 * to:
 * blastn -db '/db/est_human /db/vector' -query '12345.fa' -evalue 1e-5 > blast-wq-XXXX.out
 */
int augument_cmd(char **cmd, char *local_infile, char *remote_outfile)
{
    int alloc_len = strlen(*cmd) + strlen(remote_outfile) + 20;

    char *ptr1 = strstr(*cmd, local_infile);
    if(ptr1 == NULL) return -1;

    char *ptr2 = strstr(*cmd, basename(local_infile));
    if(ptr2 == NULL) return -1;

    while(*ptr2 != '\0')
    {
        *ptr1 = *ptr2;
        ptr1++;
        ptr2++;
    }
    *ptr1 = '\0';

    *cmd = realloc(*cmd, sizeof(char) * alloc_len);
    snprintf(*cmd + strlen(*cmd), alloc_len, " > %s", remote_outfile);

    return 0;
}


int create_task(struct work_queue *q, char *cmd_str, char *local_infile, char *local_outfile)
{
    struct work_queue_task *t;

    // Create task off the cmd
    t = work_queue_task_create(cmd_str);
    if(t == NULL) return -1;

    // Specify input & output files
    work_queue_task_specify_file(t, local_infile, basename(local_infile), WORK_QUEUE_INPUT, WORK_QUEUE_NOCACHE);
    work_queue_task_specify_file(t, local_outfile, basename(local_outfile), WORK_QUEUE_OUTPUT, WORK_QUEUE_NOCACHE);

    // Submit task
    int taskid = work_queue_submit(q, t);

    printf("submitted task (id# %d): %s\n", taskid, t->command_line);
    return 0;
}

int wait_result(struct work_queue *q)
{
    // Check if result of any task returns, and send task to workers
    struct work_queue_task *t = work_queue_wait(q, 0);

    // If a task returns
    if(t)
    {
        printf("task (id# %d) complete: %s (return code %d)\n", t->taskid, t->command_line, t->return_status);
        if(t->return_status != 0)
        {
            // The task failed. Error handling (e.g., resubmit with new parameters) here. 
        }

        work_queue_task_delete(t);
    }

    return 0;
}

