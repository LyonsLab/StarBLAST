require 'sequenceserver'

SequenceServer::DOTDIR = "/var/www/sequenceserver/.sequenceserver"
SequenceServer.init :config_file => "/var/www/sequenceserver/.sequenceserver.conf"
run SequenceServer
