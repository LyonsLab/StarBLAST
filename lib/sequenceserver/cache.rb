require 'redis'

module SequenceServer
        # Cache the result of a BLAST search
        #
        # Search param and jobid of previously run job are stored in cache.
        # Search param is the key and jobid is the value
        #
        # Each time before a job is added into the job queue, look up in
        # cache to check if this search has been run before.
        # If the job has been run before, redirect to the previous jobid
        class Cache
            def initialize(port = 6379)
                begin
                    @redis = Redis.new(:port => port, :timeout => 1)
                rescue StandardError => e
                    e.message
                end
            end

            # Query the cache
            def exist?(key)
                #key = key_from_params(params)
                begin
                    jobid = @redis.get(key)
                    puts jobid
                rescue StandardError => e
                    e.message
                end
                jobid
            end

            # Insert the job into cache
            def insert(job)
                @redis.set(job.cache_key, job.id)
            end

            # Flush the redis instance
            def flush
                begin
                    @redis.flushall
                rescue StandardError => e
                    e.message
                end
            end
        end
end