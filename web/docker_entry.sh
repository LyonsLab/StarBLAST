#!/bin/bash

# irsync the database 
iinit
irsync -rK i:$IRODS_SYNC_PATH $SEQSERVER_DB_PATH

# redis for cache
redis-server &
echo $! > $SEQSERVER_BASE_PATH/redis.pid

# store irsync path
echo $IRODS_SYNC_PATH > $SEQSERVER_SYNC_PATH_FILE

# -num_threads for blast
echo ":num_threads: $BLAST_NUM_THREADS" >> $SEQSERVER_CONFIG_FILE

# workqueue master
if [ -z $WORKQUEUE_PASSWORD ]; then
    if [ -z $PROJECT_NAME ]; then
        blast_workqueue-backend &
    else
        blast_workqueue-backend --project $PROJECT_NAME &
    fi
else
    if [ -z $PROJECT_NAME ]; then
        echo $WORKQUEUE_PASSWORD > $SEQSERVER_BASE_PATH/wq_password.txt
        blast_workqueue-backend --password $SEQSERVER_BASE_PATH/wq_password.txt &
    else
        echo $WORKQUEUE_PASSWORD > $SEQSERVER_BASE_PATH/wq_password.txt
        blast_workqueue-backend --password $SEQSERVER_BASE_PATH/wq_password.txt --project $PROJECT_NAME &
    fi
fi
echo $! > $SEQSERVER_BASE_PATH/backend.pid

# sequenceserver
bundle exec passenger start

