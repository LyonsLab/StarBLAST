#!/bin/bash

function quit_err {
    echo "ERROR"
    echo "=================="
    kill $worker_pid
    kill $backend_pid
    exit 1
}

echo "=================="
echo "TEST SUITE-1"
echo "=================="
work_queue_worker localhost 9123 1> worker.log &
worker_pid=$!
echo "worker pid: $worker_pid"
blast_workqueue-backend 1> backend.log &
backend_pid=$!
echo "backend pid: $backend_pid"

sleep 1
echo "=================="


echo "----- case 01 -----"
blast_workqueue "blastn -db '$(pwd)/db/vector' -query 's1.fa' -evalue 1e-5 -num_threads 2" 1> frontend.log
rc=$?
echo "exit status:" $rc
test $rc == 0 || quit_err
echo "-------------------"

echo "----- case 02 -----"
blast_workqueue "blastn -db '$(pwd)/db/vector' -query 's1.fa'" 1> frontend.log
rc=$?
echo "exit status:" $rc
test $rc == 0 || quit_err
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

