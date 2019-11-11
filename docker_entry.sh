#!/bin/bash

# start redis
redis-server &
echo $! > $SEQSERVER_BASE_PATH/redis.pid

# iRODs path
echo $IRODS_SYNC_PATH > $SEQSERVER_SYNC_PATH_FILE

# blast threads
echo ":num_threads: $BLAST_NUM_THREADS" >> $SEQSERVER_CONFIG_FILE

# workqueue master
if [ -z $WORKQUEUE_PASSWORD ]; then
    blast_workqueue-backend &
else
    echo $WORKQUEUE_PASSWORD > $SEQSERVER_BASE_PATH/wq_password.txt
    blast_workqueue-backend $SEQSERVER_BASE_PATH/wq_password.txt &
fi
echo $! > $SEQSERVER_BASE_PATH/backend.pid

# start passenger web server
bundle exec passenger start


