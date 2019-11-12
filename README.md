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
./blast_workqueue-backend --password <pwd-file>
./blast_workqueue-backend --unix-sock <unix-sock-path>
./blast_workqueue-backend --ip <ip-to-listen-on> --port <port>
./blast_workqueue-backend --help
```

## Communication between Frontend and Backend
Commuication is done via socket, (support both unix socket and tcp socket).

Use Unix socket by default, with a path of ```"/var/www/sequenceserver/backend.server"```

## Current Limitation/Caveat
* The input cmd from frontend to backend and the output from backend to frontend are both as a file in /tmp, only the filename is send via socket, there is no clean up done by the frontend, so user or system need to clean up /tmp regularly. Plan to having the output send to frontend via socket.

* Currently there is no sanitizing done by this program, so any valid blast option will be passed as is to work_queue_worker for execution. If sanitization is required for your system, plz do so before calling the frontend.
