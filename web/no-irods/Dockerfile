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
    SEQSERVER_APP_PATH=/var/www/sequenceserver/app \
    SEQSERVER_JOB_PATH=/var/www/sequenceserver/.sequenceserver \
    SEQSERVER_CONFIG_FILE=/var/www/sequenceserver/.sequenceserver.conf \
    SEQSERVER_DB_PATH=/var/www/sequenceserver/db


# Update and Upgrade
RUN apt-get -qq -y update \
    # Install our PGP key and add HTTPS support for APT
    && apt-get -qq -y install dirmngr gnupg \
    && apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 561F9B9CAC40B2F7 | grep -q - \
    && apt-get -qq -y install apt-transport-https ca-certificates \
    # Add Passenger APT repository
    && sh -c 'echo deb https://oss-binaries.phusionpassenger.com/apt/passenger bionic main > /etc/apt/sources.list.d/passenger.list' \
    && apt-get -qq -y update \
    # Install Passenger + Nginx module
    && apt-get -qq -y install build-essential ruby ruby-dev zlib1g-dev curl wget git \
    # Install bundler
    && gem install bundler

WORKDIR /root

# Download SequenceServer
RUN git clone https://github.com/zhxu73/sequenceserver-scale.git \
    && cd sequenceserver-scale \
    # Install the dependencies of SequenceServer
    && bundle install --path vendor/bundle --without development test \
    # Install BLAST
    && cd /root \
    && wget https://github.com/zhxu73/sequenceserver-scale-docker/releases/download/ncbi-blast-2.9.0%2B/ncbi-blast-2.9.0+-x64-linux.tar.gz \
    && tar -xvf ncbi-blast-2.9.0+-x64-linux.tar.gz \
    # Install cctools (WorkQueue)
    && wget http://ccl.cse.nd.edu/software/files/cctools-7.0.21-source.tar.gz \
    && tar -xvf cctools-7.0.21-source.tar.gz \
    && cd cctools-release-7.0.21 \
    && ./configure --prefix $HOME/cctools \
    && make \
    && make install \
    # Install blast-workqueue
    && cd /root \
    && git clone https://github.com/zhxu73/blast-workqueue.git \
    && cd blast-workqueue/src \
    && make \
    && make install

RUN mkdir -p to_final_image/bin \
    && cp ncbi-blast-2.9.0+/bin/* to_final_image/bin/ \
    && cp cctools/bin/* to_final_image/bin/ \
    && cp /usr/bin/blast_workqueue to_final_image/bin/ \
    && cp /usr/bin/blast_workqueue-backend to_final_image/bin/

# Docker entry file
ADD docker_entry.sh sequenceserver-scale/
# Add config file
ADD .sequenceserver.conf to_final_image/


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
    BLAST_NUM_THREADS=4 \
    # Username
    SEQSERVER_USER=seqserver \
    SEQSERVER_GROUP=seqserver_group \
    # Path
    SEQSERVER_BASE_PATH=/var/www/sequenceserver \
    SEQSERVER_APP_PATH=/var/www/sequenceserver/app \
    SEQSERVER_JOB_PATH=/var/www/sequenceserver/.sequenceserver \
    SEQSERVER_CONFIG_FILE=/var/www/sequenceserver/.sequenceserver.conf \
    SEQSERVER_DB_PATH=/var/www/sequenceserver/db

# Update and Upgrade
RUN apt-get -qq -y update \
    # Install our PGP key and add HTTPS support for APT
    && apt-get -qq -y install dirmngr gnupg \
    && apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 561F9B9CAC40B2F7 | grep -q - \
    && apt-get -qq -y install apt-transport-https ca-certificates \
    # Add Passenger APT repository
    && sh -c 'echo deb https://oss-binaries.phusionpassenger.com/apt/passenger bionic main > /etc/apt/sources.list.d/passenger.list' \
    && apt-get -qq -y update \
    # Install Passenger and others
    && apt-get -qq -y install passenger ruby ruby-dev zlib1g-dev redis-server wget git \
    && gem install bundler

VOLUME ["/var/www/sequenceserver/db"]
EXPOSE 3000 9123

# Create User and Group
RUN addgroup ${SEQSERVER_GROUP} && adduser --quiet --disabled-login --gecos 'SequenceServer' ${SEQSERVER_USER} && adduser ${SEQSERVER_USER} ${SEQSERVER_GROUP}
WORKDIR /home/$SEQSERVER_USER

COPY --from=build /root/sequenceserver-scale ${SEQSERVER_APP_PATH}
COPY --from=build /root/to_final_image from_build_image

RUN cp from_build_image/bin/* /usr/bin \
    && mkdir -p ${SEQSERVER_JOB_PATH} \
    && cp from_build_image/.sequenceserver.conf $SEQSERVER_CONFIG_FILE \
    && echo ${WORKQUEUE_PASSWORD} > $SEQSERVER_BASE_PATH/wq_password.txt \
    && chown -R ${SEQSERVER_USER}:${SEQSERVER_GROUP} ${SEQSERVER_BASE_PATH}

USER seqserver
WORKDIR ${SEQSERVER_APP_PATH}
ENTRYPOINT ["/bin/bash", "docker_entry.sh"]



