#!/bin/bash

# irsync the database 
iinit
irsync -rK i:$IRODS_SYNC_PATH $SEQSERVER_DB_PATH

# store irsync path
echo $IRODS_SYNC_PATH > $SEQSERVER_SYNC_PATH_FILE

# store ip address of master/web instance
echo $MASTER_IP > $SEQSERVER_BASE_PATH/master_ip.txt

# launch work_queue_factory
if [ -z $WORKQUEUE_PASSWORD ]; then
    if [ -z $PROJECT_NAME ]; then
        /usr/bin/work_queue_factory -T local $(cat $SEQSERVER_BASE_PATH/master_ip.txt) 9123 -w $NUM_WORKER --cores $BLAST_NUM_THREADS -E "-b 3"
    else
        /usr/bin/work_queue_factory -T local -M $PROJECT_NAME -w $NUM_WORKER --cores $BLAST_NUM_THREADS -E "-b 3"
    fi
else
    if [ -z $PROJECT_NAME ]; then
        echo $WORKQUEUE_PASSWORD > $SEQSERVER_BASE_PATH/wq_password.txt
        /usr/bin/work_queue_factory -T local $(cat $SEQSERVER_BASE_PATH/master_ip.txt) 9123 -P $SEQSERVER_BASE_PATH/wq_password.txt -w $NUM_WORKER --cores $BLAST_NUM_THREADS -E "-b 3"
    else
        echo $WORKQUEUE_PASSWORD > $SEQSERVER_BASE_PATH/wq_password.txt
        /usr/bin/work_queue_factory -T local -M $PROJECT_NAME -P $SEQSERVER_BASE_PATH/wq_password.txt -w $NUM_WORKER --cores $BLAST_NUM_THREADS -E "-b 3"
    fi
fi

