/*
 * Copyright (C) 2008- The University of Notre Dame
 * This software is distributed under the GNU General Public License.
 * See the file COPYING for details.
 * */

/*
 * 
 * A trivial solution to make SequenceServer run job through Work Queue.
 * 
 * Modified from the example:
 * http://ccl.cse.nd.edu/software/manuals/work_queue_example.c
 * By: John Xu
 * Date: 10-8-2019
 * 
 * Usage:
 * blast_workqueue <sequence-file> <cmd-to-run-blast>
 * 
 * Each time a job is to be executed by SeqServer, rather than run the blast
 * program directly, it calls this program, and pass the exact cmd that will
 * be used to execute the blast search, along with the input query file.
 * 
 * The stdout and stderr of the program run by SeqServer is captured, and the
 * stdout is used for the output from blast, so logging has to be done
 * separately, and the output of blast has to be print to stdout to mimic the
 * behavior.
 * The local output file specified to work_queue_task is merely a temporary
 * file, since there is no way to print the task output directly, it has to
 * go through a file. The local output file is read and print to stdout after
 * task completion to mimic the behavior of blast invocation.
 * 
 * The reason why this is a trivial solution is that everytime a job is
 * executed this program is run, that means it has to create a work_queue
 * every single time, which means there cannot be another instance of program
 * running, because they all listen on the same port.
 * Only 1 search is supplied each run, which means despite using WorkQueue to
 * execute BLAST search, still only 1 search can be performed at a time.
 * */

#include "work_queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// Path to local temporary output file
const char *local_outfile = "/tmp/blast-wq.out";
// Path to remote(on Worker) output file
const char *remote_outfile = "blast-wq.out";


char *blast_cmd_from_cmd_arg(char *cmd_arg);
char *remote_infilename(char *local);
int print_file(const char *filename);

/*
 * Main
 */
int main(int argc, char *argv[])
{
    struct work_queue *q;
    struct work_queue_task *t;
    int port = WORK_QUEUE_DEFAULT_PORT;
    int taskid;
    int rc;

    // Check number of argument
    if(argc < 3)
    {
        fprintf(stderr, "blast_wq_example <infile> <cmd-from-sequenceserver>\n");
        fprintf(stderr, "Execute the given blast cmd on Work Queue Workers, and output to stdout\n");
        exit(1);
    }

    // Open the log file
    // stdout reserved for BLAST output, logging has to go through file
    FILE *log_file = fopen("/tmp/blast-wq-app.log", "w+");
    if(log_file == NULL)
    {
        fprintf(stderr, "Unable to open log file\n");
        exit(1);
    }

    // Create the tasks queue using the default port 9123
    q = work_queue_create(port);
    if(!q)
    {
        fprintf(stderr, "couldn't listen on port %d: %s\n", port, strerror(errno));
        fprintf(log_file, "couldn't listen on port %d: %s\n", port, strerror(errno));
        return 1;
    }

    fprintf(log_file, "listening on port %d...\n", work_queue_port(q));

    // Append the output file to th given blast cmd
    char *blast_cmd = blast_cmd_from_cmd_arg(argv[2]);
    fprintf(log_file, "cmd: %s\n", blast_cmd);

    // Create the task with the cmd
    t = work_queue_task_create(blast_cmd);


    // Specified the input file and output file of the task, the local path and remote path are different
    work_queue_task_specify_file(t, argv[1], remote_infilename(argv[1]), WORK_QUEUE_INPUT, WORK_QUEUE_NOCACHE);
    work_queue_task_specify_file(t, local_outfile, remote_outfile, WORK_QUEUE_OUTPUT, WORK_QUEUE_NOCACHE);

    // All files has been specified, we are ready to submit the task to the queue.
    taskid = work_queue_submit(q, t);

    fprintf(log_file, "submitted task (id# %d): %s\n", taskid, t->command_line);

    fprintf(log_file, "waiting for tasks to complete...\n");

    // work_queue_wait waits for the task to return.
    t = work_queue_wait(q, -1);

    if(t)
    {
        fprintf(log_file, "task (id# %d) complete: %s (return code %d)\n", t->taskid, t->command_line, t->return_status);
        if(t->return_status != 0)
        {
            // The task failed.
            fprintf(stderr, "task (id# %d) complete: %s (return code %d)\n", t->taskid, t->command_line, t->return_status);
            exit(t->return_status);
        }

        work_queue_task_delete(t);
    }

    // Print the output file to stdout, in order to mimc the behavior of
    // blast invocation by SeqServer
    rc = print_file(local_outfile);
    if(rc < 0)
    {
        fprintf(stderr, "Unable to read output file\n");
        exit(-1);
    }

    fprintf(log_file, "all tasks complete!\n");

    free(blast_cmd);
    fclose(log_file);
    work_queue_delete(q);

    return 0;
}

/*
 * BLAST cmd from cmd args
 * Append the output file to the given cmd.
 * Memory is allocated for the new cmd, need to be freed manually after use.
 * 
 * \param cmd_arg Command line argument, argv[2]
 * \return The blast command to be passed to a task
 */
char *blast_cmd_from_cmd_arg(char *cmd_arg)
{
    char *blast_cmd = NULL;
    // Allocate
    blast_cmd = calloc(strlen(cmd_arg) + strlen(remote_outfile) + 10, sizeof(char));

    // We know sizeof(blast_cmd) > strlen(cmd_arg)
    strncpy(blast_cmd, cmd_arg, strlen(cmd_arg));
    sprintf(blast_cmd + strlen(blast_cmd), " > %s", remote_outfile);

    return blast_cmd;
}

/*
 * Remote path must not be absolute per WorkQueue's requirement,
 * this function strips the filename from the absolute path given by SeqServer as the remote path.
 * 
 * \param local Local path to the input filename
 * \return char pointer that points to a charater in \p local, indicate the start of the filename (aka remote path)
 */
char *remote_infilename(char *local_path)
{
    if(local_path == NULL) return local_path;

    // Find occurance of '/' in reverse order
    char *last_slash = strrchr(local_path, '/');

    // If slash is found in local path
    if(last_slash != NULL)
        return last_slash+1;
    // Slash not found
    else
        return local_path;
        
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

