**Beta version:** 
[![new gem version](https://img.shields.io/badge/version-2.0%20(beta)-yellowgreen.svg)](http://rubygems.org/gems/sequenceserver) 


# [SequenceServer](https://github.com/wurmlab/sequenceserver) - BLAST searching made easy!

SequenceServer lets you rapidly set up a BLAST+ server with an intuitive user interface for personal or group use.

Forked from 2.0.0 beta3
https://github.com/wurmlab/sequenceserver/tree/2.0.0.beta3

Use WorkQueue as execution backend instead of local execution.
https://github.com/zhxu73/blast-workqueue

Support job caching with redis-server (currently separately on app-cache branch)

## Deploy with Passenger + Nginx
Install Ruby, Passenger, Nginx
```bash
# Install PGP and HTTPS support
sudo apt-get install -y dirmngr gnupg
sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 561F9B9CAC40B2F7
sudo apt-get install -y apt-transport-https ca-certificates

# Add APT repo
sudo sh -c 'echo deb https://oss-binaries.phusionpassenger.com/apt/passenger bionic main > /etc/apt/sources.list.d/passenger.list'
sudo apt-get update

# Install nginx passenger ruby
sudo apt-get install -y nginx libnginx-mod-http-passenger ruby ruby-dev
```
Suppose you clone the repo under `/var/www/` then your site config should look something like this:
```
server {
    listen 80;
    server_name localhost;

    root /var/www/sequenceserver-scale/public;

    # Turn on Passenger
    passenger_enabled on;
    passenger_ruby /usr/bin/ruby;
    #passenger_user seqserver;
    #passenger_group seqserver_group;
}
```

## Deploy with Passenger standalone


