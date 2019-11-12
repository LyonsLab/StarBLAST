#######################################################
#
# Build Image
#
#######################################################

FROM ubuntu AS build

ENV SEQSERVER_USER=seqserver \
    SEQSERVER_GROUP=seqserver_group \
    # Path
    SEQSERVER_BASE_PATH=/var/www/sequenceserver \
    SEQSERVER_DB_PATH=/var/www/sequenceserver/db


# Update and Upgrade
RUN apt-get -qq -y update \
    && apt-get -qq -y install build-essential zlib1g-dev wget git
WORKDIR /root

# Download config file
RUN git clone https://github.com/JLHonors/ACIC2019-Midterm.git \
    # Install BLAST
    && wget ftp://ftp.ncbi.nlm.nih.gov/blast/executables/blast+/LATEST/ncbi-blast-2.9.0+-x64-linux.tar.gz \
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
    && cp ACIC2019-Midterm/deploy/sync_blast_db.sh to_final_image/


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
    WORKQUEUE_PASSWORD= \
    MASTER_IP=127.0.0.1 \
    BLAST_NUM_THREADS=4 \
    NUM_WORKER=1 \
    # Username
    SEQSERVER_USER=seqserver \
    SEQSERVER_GROUP=seqserver_group \
    # Path
    SEQSERVER_BASE_PATH=/var/www/sequenceserver \
    SEQSERVER_DB_PATH=/var/www/sequenceserver/db

VOLUME ["/var/www/sequenceserver/db"]
EXPOSE 9123

WORKDIR /root
COPY --from=build /root/to_final_image from_build_image

# Update and Upgrade
RUN apt-get -qq -y update \
    # Create User and Group
    && addgroup ${SEQSERVER_GROUP} && adduser --quiet --disabled-login --gecos 'SequenceServer' ${SEQSERVER_USER} && adduser ${SEQSERVER_USER} ${SEQSERVER_GROUP} \
    && cd /root \
    # Copy files
    && cp from_build_image/bin/* /usr/bin \
    && echo ${WORKQUEUE_PASSWORD} > $SEQSERVER_BASE_PATH/wq_password.txt \
    && chown -R ${SEQSERVER_USER}:${SEQSERVER_GROUP} ${SEQSERVER_BASE_PATH}
ADD docker_entry.sh ${SEQSERVER_BASE_PATH}/docker_entry.sh
RUN chown -R ${SEQSERVER_USER}:${SEQSERVER_GROUP} ${SEQSERVER_BASE_PATH}

#USER ${SEQSERVER_USER}
WORKDIR ${SEQSERVER_BASE_PATH}
ENTRYPOINT ["/bin/bash", "docker_entry.sh"]




