#!/bin/bash

#set -e
#set -o errexit ; set -o nounset

function quit_err {
    echo "ERROR"
    echo "=================="
    kill $worker_pid
    kill $backend_pid
    exit 1
}

echo "=================="
echo "TEST SUITE-2"
echo "=================="
work_queue_worker localhost 9123 1> worker.log &
worker_pid=$!
echo "worker pid: $worker_pid"
blast_workqueue-backend 1> backend.log &
backend_pid=$!
echo "backend pid: $backend_pid"

echo "=================="

sleep 1


echo "----- case 01 -----"
blast_workqueue "blastn -db '$(pwd)/db/DO_NOT_EXIST' -query 's1.fa' -evalue 1e-5"
rc=$?
echo "exit status:" $rc
test $rc == 255 || quit_err
echo "-------------------"

echo "----- case 02 -----"
blast_workqueue "blastn -db '$(pwd)/db/DO_NOT_EXIST1' -query 'DO_NOT_EXIST/s1.fa' -evalue 1e-5"
rc=$?
echo "exit status:" $rc
test $rc == 255 || quit_err
echo "-------------------"

echo "----- case 03 -----"
blast_workqueue "blastn -db '$(pwd)/db/DO_NOT_EXIST1' -query 'DO_NOT_EXIST' -evalue 1e-5"
rc=$?
echo "exit status:" $rc
test $rc == 255 || quit_err
echo "-------------------"


timeout 2 wait $backend_pid
rc=$?
echo "wait backend exit status:" $rc
test $rc == 127 || quit_err

timeout 2 wait $worker_pid
rc=$?
echo "wait worker exit status:" $rc
test $rc == 127 || quit_err

echo "=================="
echo "force kill worker"
kill $worker_pid
echo "force kill backend"
kill $backend_pid

exit 0

