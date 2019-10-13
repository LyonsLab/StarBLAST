# blast-workqueue

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
```bash
./blast_workqueue "blastn -db /var/db/est_human -query /tmp/query.fa -evalue 1e-5"
```

## Backend
Constantly running, upon receiving msg from a frontend, create a WorkQueue task and dispatch it to work_queue_worker connect to the backend.
