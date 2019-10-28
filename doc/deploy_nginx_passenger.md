# Deploy with Passenger + Nginx
## Install Ruby, Passenger, Nginx.
[Reference Guide from Passenger](https://www.phusionpassenger.com/library/walkthroughs/deploy/)
```bash
# Install PGP and HTTPS support
sudo apt-get install -y dirmngr gnupg
sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 561F9B9CAC40B2F7
sudo apt-get install -y apt-transport-https ca-certificates

# Add APT repo
sudo sh -c 'echo deb https://oss-binaries.phusionpassenger.com/apt/passenger bionic main > /etc/apt/sources.list.d/passenger.list'
sudo apt-get update

# Install nginx passenger ruby
sudo apt-get install -y nginx libnginx-mod-http-passenger ruby ruby-dev git wget curl build-essential redis-server
```
## Download sequenceserver-scale
`git clone https://github.com/zhxu73/sequenceserver-scale.git /var/www/sequenceserver/app`

## Config file for sequenceserver
Place your config file at `/var/www/sequenceserver/.sequenceserver.conf`
```
---
:host: 0.0.0.0
:port: 4567
:options:
  :blastn:
  - "-task blastn"
  - "-evalue 1e-5"
  :blastp:
  - "-evalue 1e-5"
  :blastx:
  - "-evalue 1e-5"
  :tblastx:
  - "-evalue 1e-5"
  :tblastn:
  - "-evalue 1e-5"
:num_threads: 2
:bin: "/usr/bin"
:database_dir: "/var/www/sequenceserver/db"

```

## Install BLAST
Despite the execution backend switch to WorkQueue, some BLAST utilities are still needed for sequenceserver
```
cd ~/
wget ftp://ftp.ncbi.nlm.nih.gov/blast/executables/blast+/LATEST/ncbi-blast-2.9.0+-x64-linux.tar.gz
tar -xvf ncbi-blast-2.9.0+-x64-linux.tar.gz
sudo cp ncbi-blast-2.9.0+/bin/* /usr/bin
```

## Install [blast-workqueue](https://github.com/zhxu73/blast-workqueue)
Install cctools library, which is required for compile blast-workqueue, if you have not
```bash
cd ~/
wget http://ccl.cse.nd.edu/software/files/cctools-7.0.19-x86_64-centos7.tar.gz
tar -xvf cctools-7.0.19-x86_64-centos7.tar.gz
mv cctools-7.0.19-x86_64-centos7 cctools
sudo cp cctools/bin/* /usr/bin
```
Compile and install blast-workqueue
```bash
git clone https://github.com/zhxu73/blast-workqueue
cd blast-workqueue/src
make
make install
```

## Download sample BLAST database
Sequenceserver require at least one database in the db path to function, so either download one like below, or upload your own.
```
cd ~/
curl ftp://ftp.ncbi.nlm.nih.gov/blast/db/vector.tar.gz -O
sudo tar -xvf vector.tar.gz -C /var/www/sequenceserver/db/
rm ~/vector.tar.gz
```
## Nginx site config
Suppose you clone the repo like above then your site config should look something like below.
```
server {
    listen 80;
    server_name localhost;

    root /var/www/sequenceserver/app/public;

    # Turn on Passenger
    passenger_enabled on;
    passenger_ruby /usr/bin/ruby;
    #passenger_user seqserver;
    #passenger_group seqserver_group;
}
```
Copy your config file to `/etc/nginx/sites-enabled/sequenceserver.config` and run
```bash
sudo nginx -s reload
sudo systemctl restart nginx
```
## Start blast_workqueue-backend
By default the backend listen on a unix socket(`"/var/www/sequenceserver/backend.server"`), make sure the user have permission to it.
```
blast_workqueue-backend
```
## Start a WorkQueue worker locally for testing
```
work_queue_worker localhost 9123
```



