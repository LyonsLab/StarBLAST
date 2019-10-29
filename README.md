# blast-workqueue

[![Build Status](https://travis-ci.com/zhxu73/blast-workqueue.svg?branch=master)](https://travis-ci.com/zhxu73/blast-workqueue)

Integrate WorkQueue with SequenceServer.

Currently it still require some modification on the SequenceServer code.
Modify `command` method in `lib/sequenceserver/blast/job.rb`
from:
```ruby
@command ||= "#{method} -db '#{databases.map(&:name).join(' ')}'" \
             " -query '#{qfile}' #{options}"
```
to:
```ruby
@command ||= "blast_workqueue \"#{method} -db '#{databases.map(&:name).join(' ')}'" \
             " -query '#{qfile}' #{options}\""
```

## Frontend
Called for every single BLAST search:

Usage:
```bash
./blast_workqueue "blastn -db /var/db/est_human -query /tmp/query.fa -evalue 1e-5"
```

## Backend
Constantly running, upon receiving msg from a frontend, create a WorkQueue task and dispatch it to work_queue_worker connect to the backend.

A password file can be used to authenticate between work_queue_worker and the backend.

Usage:
```bash
./blast_workqueue-backend
./blast_workqueue-backend <pwd-file>
./blast_workqueue-backend <pwd-file> <unix-sock-path>
./blast_workqueue-backend <pwd-file> <ip> <port>
```

## Communication between Frontend and Backend
Commuication is done via socket, (support both unix socket and tcp socket).

Use Unix socket by default, with a path of ```"/var/www/sequenceserver/backend.server"```

