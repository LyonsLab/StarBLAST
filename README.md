# sequenceserver-scale-docker
docker container for (sequenceserver-scale)[https://github.com/zhxu73/sequenceserver-scale]

## Web/Master docker image
https://hub.docker.com/repository/docker/zhxu73/sequenceserver-scale

### Available Environment Variable:

* `PROJECT_NAME`

    This option utilize the catalog server

    Default to empty (not set)

* `WORKQUEUE_PASSWORD`

    Password between master and worker

    Use `-e WORKQUEUE_PASSWORD=` to set to empty (no password)

* `BLAST_NUM_THREADS`

    Number of threads the blast program will be running with

    Should be consistent with the value set for worker container

    Default to 4

* `SEQSERVER_DB_PATH`

    Path of database inside the container, should be consistent between master and worker

    Default to `/var/www/sequenceserver/db`

* `IRODS_SYNC_PATH`

    iRODS (tag latest) container only

    The iRODS path to download the database from.

    Default to `/iplant/home/shared/iplantcollaborative/example_data/GEA_Blast_dbs`

### Example Usage:
With iRODS
```
docker run --rm -ti -p 80:3000 -p 9123:9123 -e PROJECT_NAME=starBLAST -e WORKQUEUE_PASSWORD= -e BLAST_NUM_THREADS=4 zhxu73/sequenceserver-scale
```
```
docker run --rm -ti -p 80:3000 -p 9123:9123 -e PROJECT_NAME=starBLAST -e WORKQUEUE_PASSWORD= -e BLAST_NUM_THREADS=4 -e SEQSERVER_DB_PATH=/home/zhxu73/db zhxu73/sequenceserver-scale
```

Without iRODS
```
docker run --rm -ti -p 80:3000 -p 9123:9123 -e PROJECT_NAME=starBLAST -e WORKQUEUE_PASSWORD= -e BLAST_NUM_THREADS=4 -v /local_db_path:/var/www/sequenceserver/db zhxu73/sequenceserver-scale:no-irods
```
```
docker run --rm -ti -p 80:3000 -p 9123:9123 -e PROJECT_NAME=starBLAST -e WORKQUEUE_PASSWORD= -e BLAST_NUM_THREADS=4 -e SEQSERVER_DB_PATH=/home/zhxu73/db -v $HOME/blastdb:/home/zhxu73/db zhxu73/sequenceserver-scale:no-irods
```

## Worker docker image
https://hub.docker.com/repository/docker/zhxu73/sequenceserver-scale-worker


### Available Environment Variable:

* `PROJECT_NAME`

    Default to empty (not set), if set (not empty), then MASTER_IP will be ignored

* `MASTER_IP`

    IP address of the master

    Default to `127.0.0.1` (localhost)

* `WORKQUEUE_PASSWORD`

    Password between master and worker

* `BLAST_NUM_THREADS`

    Number of threads the blast program will be running with

    Default to 4

* `NUM_WORKER`

    Number of `work_queue_worker` to be spawned by the `work_queue_factory`

    `NUM_WORKER` * `BLAST_NUM_THREADS` should not exceed the number of CPU/cores the instance (that runs the worker container) have

    Default to 1

* `SEQSERVER_DB_PATH`

    iRODS (tag latest) container only

    Path of database inside the container, should be consistent between master and worker

    Default to `/var/www/sequenceserver/db`

* `IRODS_SYNC_PATH`

    iRODS (tag latest) container only

    The iRODS path to download the database from.

    Default to `/iplant/home/shared/iplantcollaborative/example_data/GEA_Blast_dbs`

### Example Usage:
With iRODS
```
docker run --rm -ti --net=host -e PROJECT_NAME=starBLAST -e WORKQUEUE_PASSWORD= -e BLAST_NUM_THREADS=4 -e NUM_WORKER=2 zhxu73/sequenceserver-scale-worker
```
```
docker run --rm -ti --net=host -e PROJECT_NAME=starBLAST -e WORKQUEUE_PASSWORD= -e BLAST_NUM_THREADS=4 -e NUM_WORKER=2 -e SEQSERVER_DB_PATH=/home/zhxu73/db zhxu73/sequenceserver-scale-worker
```

Without iRODS
```
docker run --rm -ti --net=host -e PROJECT_NAME=starBLAST -e WORKQUEUE_PASSWORD= -e BLAST_NUM_THREADS=4 -e NUM_WORKER=2 -v /local_db_path:/var/www/sequenceserver/db zhxu73/sequenceserver-scale-worker:no-irods
```
