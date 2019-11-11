#!/bin/bash


redis-server &
echo $! > $SEQSERVER_BASE_PATH/redis.pid

echo $IRODS_SYNC_PATH > $SEQSERVER_SYNC_PATH_FILE

echo ":num_threads: $BLAST_NUM_THREADS" >> $SEQSERVER_CONFIG_FILE

if [ -z $WORKQUEUE_PASSWORD ]; then
    blast_workqueue-backend &
else
    echo $WORKQUEUE_PASSWORD > $SEQSERVER_BASE_PATH/wq_password.txt
    blast_workqueue-backend $SEQSERVER_BASE_PATH/wq_password.txt &
fi
echo $! > $SEQSERVER_BASE_PATH/backend.pid
bundle exec passenger start

