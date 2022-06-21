## Concurrency
- Primitives:
  - Locks: deal with data races
  - Condition Variables: deal with races related to control
    - wait, signal 
  - Semaphore
- Producer/Consumer problem: makes use of a bounded queue. The queue might be full or empty in which in either cases we have to wait until the condition changes in-terms of the aspect of the producers or consumers.

      Producer/Consumer problem:
      Let's consider the solution making use of two condition variables namely fill and empty which makes sure that the producer is only woken up by the consumer and vice versa for the consumer. And we make use of while so that we re-check the condition (the condition might have changed from the time when the signal took place)
      
      void *producer(void *arg) {
        for (int i = 0; i < loops; i++) {
          Mutex_lock(&m);
          while (numfull == max)
             Cond_wait(&empty, &m);
          fill_queue(i);
          Cond_signal(&fill);
          Mutex_unlock(&m);
        }
      }
      
      void *consumer(void *arg) {
        while (#some condition to create multiple consumers) {
          Mutex_lock(&m);
          while (numfull == 0)
            Cond_wait(&fill, &m);
          int temp = get_from_queue();
          Cond_signal(&empty);
          Mutex_unlock(&m);
          printf("%d", temp);
        }
      }
          
