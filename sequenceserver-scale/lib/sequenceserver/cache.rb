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
    class << self
      def enabled?
        return SequenceServer::config[:cache] == "enable"
      end
    end

    def initialize(port = 6379)
      return unless Cache.enabled?

      return unless @redis.nil?

      begin
        @redis = Redis.new(:port => port, :timeout => 1)
        # Flush all upon startup
        @redis.flushall
        SequenceServer::logger.info("Redis Cache initialized")
      rescue StandardError => e
        SequenceServer::logger.debug(e.message)
      end
    end

    # Query the cache
    def exist?(key)
      return nil if @redis.nil?

      begin
        jobid = @redis.get(key)
      rescue Redis::BaseError => e
        SequenceServer::logger.debug(e.message)
        jobid = nil
      end
      jobid
    end

    def set_not_exist(key, value)
      return nil if @redis.nil?

      begin
        return @redis.setnx(key, value)
      rescue Redis::BaseError => e
        SequenceServer::logger.debug(e.message)
      end
    end

    # Insert the job into cache
    def insert(job)
      return if @redis.nil?

      # redis has a max length of 512MB
      return if job.cache_key.length >= 536870912

      begin
        # Expire 10 min sooner than job file, at least 1 minute
        expire_time = [JobRemover::DEFAULT_JOB_LIFETIME * 60 - 600, 60].max
        @redis.set(job.cache_key, job.id, :ex => expire_time)
      rescue StandardError => e
        SequenceServer::logger.debug(e.message)
      end
    end

    # Insert the job into cache
    def insert(key, value)
      return if @redis.nil?

      # redis has a max length of 512MB
      return if key.length >= 536870912

      begin
        # Expire 10 min sooner than job file, at least 1 minute
        expire_time = [JobRemover::DEFAULT_JOB_LIFETIME * 60 - 600, 60].max
        @redis.set(key, value, :ex => expire_time)
      rescue StandardError => e
        SequenceServer::logger.debug(e.message)
      end
    end

    # Remove job from cache
    def remove(job)
      return if @redis.nil?

      begin
        @redis.del(job.cache_key, job.id)
      rescue StandardError => e
        SequenceServer::logger.debug(e.message)
      end
    end

    # Flush the redis instance
    def flush
      return if @redis.nil?

      begin
        @redis.flushall
      rescue StandardError => e
        SequenceServer::logger.debug(e.message)
      end
    end
  end
end

