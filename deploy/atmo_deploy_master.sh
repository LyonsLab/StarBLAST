#!/bin/bash


WORKQUEUE_PASSWORD=
PROJECT_NAME=
CPU_PER_WORKER=8

DATABASE=nt_v5


if [ -z $PROJECT_NAME ]; then
    PROJECT_NAME="$ATMO_USER-starBLAST"
fi
if [ -z $WORKQUEUE_PASSWORD ]; then
    WORKQUEUE_PASSWORD="$(echo $PROJECT_NAME | sha1sum | cut -d' ' -f1)"
fi

DEBIAN_FRONTEND=noninteractive apt-get -qq -y update
DEBIAN_FRONTEND=noninteractive apt-get -qq -y install wget

command -v docker
if [ $? != 0 ]; then
    ezd
fi

#
# Download progress script
git clone https://github.com/zhxu73/sequenceserver-scale-docker.git
cp sequenceserver-scale-docker/deploy/starBLAST-master.service /etc/systemd/system
rm -rf sequenceserver-scale-docker

#
# Environment variable
echo "PROJECT_NAME=$PROJECT_NAME" >> /etc/starBLAST-environment
echo "WORKQUEUE_PASSWORD=$WORKQUEUE_PASSWORD" >> /etc/starBLAST-environment
echo "CPU_PER_WORKER=$CPU_PER_WORKER" >> /etc/starBLAST-environment
echo "DATABASE=$DATABASE" >> /etc/starBLAST-environment

echo "source /etc/starBLAST-environment" >> /etc/profile
source /etc/profile

#
# Pull docker container
docker pull ncbi/blast
docker pull zhxu73/sequenceserver-scale:no-irods

#
# Permission for /scratch
mkdir /scratch
chown -R $ATMO_USER /scratch
chmod u=7 /scratch

#
# Download DB via NCBI docker (GCP)
docker run --rm \
  -v /scratch:/blast/blastdb:rw \
  -w /blast/blastdb \
  ncbi/blast \
  update_blastdb.pl --source gcp $DATABASE

systemctl daemon-reload
systemctl enable starBLAST-master
systemctl restart starBLAST-master


