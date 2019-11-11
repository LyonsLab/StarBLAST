#!/bin/bash

echo $MASTER_IP > $SEQSERVER_BASE_PATH/master_ip.txt

if [ -z $WORKQUEUE_PASSWORD ]; then
    /usr/bin/work_queue_worker $SEQSERVER_BASE_PATH/master_ip.txt 9123
else
    echo $WORKQUEUE_PASSWORD > $SEQSERVER_BASE_PATH/wq_password.txt
    /usr/bin/work_queue_worker $(cat $SEQSERVER_BASE_PATH/master_ip.txt) 9123 -P $SEQSERVER_BASE_PATH/wq_password.txt
fi
