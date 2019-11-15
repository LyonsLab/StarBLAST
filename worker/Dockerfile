#######################################################
#
# Build Image
#
#######################################################

FROM ubuntu AS build

ENV IRODS_USER=anonymous \
    SEQSERVER_USER=seqserver \
    SEQSERVER_GROUP=seqserver_group \
    # Path
    SEQSERVER_BASE_PATH=/var/www/sequenceserver \
    SEQSERVER_DB_PATH=/var/www/sequenceserver/db \
    SEQSERVER_SYNC_PATH_FILE=/var/www/sequenceserver/irods_sync_path.txt


# Update and Upgrade
RUN apt-get -qq -y update \
    && apt-get -qq -y install build-essential zlib1g-dev wget git
WORKDIR /root

# Download config file
RUN git clone https://github.com/JLHonors/ACIC2019-Midterm.git \
    # Install BLAST
    && wget https://github.com/zhxu73/sequenceserver-scale-docker/releases/download/ncbi-blast-2.9.0%2B/ncbi-blast-2.9.0+-x64-linux.tar.gz \
    && tar -xvf ncbi-blast-2.9.0+-x64-linux.tar.gz
# Install cctools (WorkQueue)
RUN wget http://ccl.cse.nd.edu/software/files/cctools-7.0.21-source.tar.gz \
    && tar -xvf cctools-7.0.21-source.tar.gz \
    && cd cctools-release-7.0.21 \
    && ./configure --prefix $HOME/cctools \
    && make \
    && make install

RUN mkdir -p to_final_image/bin \
    && cp ncbi-blast-2.9.0+/bin/* to_final_image/bin/ \
    && cp cctools/bin/* to_final_image/bin/ \
    && cp ACIC2019-Midterm/deploy/sync_blast_db.sh to_final_image/ \
    && echo "{ \"irods_zone_name\": \"iplant\", \"irods_host\": \"data.cyverse.org\", \"irods_port\": 1247, \"irods_user_name\": \"$IRODS_USER\" }" > to_final_image/irods_environment.json


#######################################################
#
# Final Image
#
#######################################################
FROM ubuntu

LABEL Description="Scalable intuitive local web frontend for the BLAST bioinformatics tool"
LABEL Version="0.1"

# Argument
ENV PROJECT_NAME= \
    WORKQUEUE_PASSWORD=VERY_STRONG_PASSWORD \
    IRODS_SYNC_PATH=/iplant/home/shared/iplantcollaborative/example_data/GEA_Blast_dbs \
    MASTER_IP=127.0.0.1 \
    BLAST_NUM_THREADS=4 \
    NUM_WORKER=1 \
    # Username
    IRODS_USER=anonymous \
    SEQSERVER_USER=seqserver \
    SEQSERVER_GROUP=seqserver_group \
    # Path
    SEQSERVER_BASE_PATH=/var/www/sequenceserver \
    SEQSERVER_DB_PATH=/var/www/sequenceserver/db \
    SEQSERVER_SYNC_PATH_FILE=/var/www/sequenceserver/irods_sync_path.txt

EXPOSE 9123

WORKDIR /root
COPY --from=build /root/to_final_image from_build_image

# Update and Upgrade
RUN apt-get -qq -y update \
    && apt-get -qq -y install gnupg wget \
    # Install iRODS
    && wget -qO - https://packages.irods.org/irods-signing-key.asc | apt-key add - \
    && echo "deb [arch=amd64] https://packages.irods.org/apt/ xenial main" | tee /etc/apt/sources.list.d/renci-irods.list \
    && apt-get -qq -y update \
    && apt-get -qq -y install irods-icommands \
    # Create User and Group
    && addgroup ${SEQSERVER_GROUP} && adduser --quiet --disabled-login --gecos 'SequenceServer' ${SEQSERVER_USER} && adduser ${SEQSERVER_USER} ${SEQSERVER_GROUP} \
    && cd /root \
    # Copy files
    && cp from_build_image/bin/* /usr/bin \
    && mkdir -p $SEQSERVER_DB_PATH \
    && echo ${IRODS_SYNC_PATH} > $SEQSERVER_SYNC_PATH_FILE \
    && cp from_build_image/sync_blast_db.sh $SEQSERVER_BASE_PATH/ \
    && mkdir -p /home/$SEQSERVER_USER/.irods \
    && cp from_build_image/irods_environment.json /home/$SEQSERVER_USER/.irods/irods_environment.json \
    && echo ${WORKQUEUE_PASSWORD} > $SEQSERVER_BASE_PATH/wq_password.txt \
    && cd ${SEQSERVER_APP_PATH} \
    && chown -R ${SEQSERVER_USER}:${SEQSERVER_GROUP} ${SEQSERVER_BASE_PATH} \
    # Allow folder creation in /home
    && chown :${SEQSERVER_GROUP} /home \
    && chmod g+rwx /home

ADD docker_entry.sh ${SEQSERVER_BASE_PATH}/docker_entry.sh
RUN chown -R ${SEQSERVER_USER}:${SEQSERVER_GROUP} ${SEQSERVER_BASE_PATH}

USER ${SEQSERVER_USER}
WORKDIR ${SEQSERVER_BASE_PATH}
ENTRYPOINT ["/bin/bash", "docker_entry.sh"]




