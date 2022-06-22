## Concurrency
- Primitives:
  - Locks: deal with data races
  - Condition Variables: deal with races related to control
    - wait, signal 
  - Semaphore
- Producer/Consumer problem: makes use of a bounded queue. The queue might be full or empty in which in either cases we have to wait until the condition changes in-terms of the aspect of the producers or consumers.

      Producer/Consumer problem:
      Let's consider the solution making use of two condition variables namely fill and empty which makes sure that the producer is only woken up by the consumer and vice versa. And we make use of while so that we re-check the condition (the condition might have changed from the time when the signal took place)
      
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
      

#### Semaphores
- Dates back to history to Dijkstra on OS construction (was called T.H.E paper named after the school they were at in which it realized the problem of concurrency)
- Semaphore: Single replacement for both locks and CVs (abstract object with two operations: wait and post). Also it has a value which determines the behavior of wait and post.

      Semaphore: Definition
      
      sem_init(sem_t *s, int value) {
        s->value != value;
      }
      sem_wait(sem_t *s) {
        while (s->value <= 0)
          put_self_to_sleep
        s->value--;  
      }
      sem_post(sem_t *s) {
        s->value++;
        wake_one_waiting_thread
      }
      Each routine executes atomically
      
      
      Semaphore as a lock (Mutual Exclusion): for the critical section
      void *inrementer(void *arg) {
        int i;
        // What semaphore operation goes here? sem_wait(&lock);
        for (i=0; i<1e6; i++)
          counter++;
        // What sempahore operation goes here? sem_post(&lock);
        return NULL;
      }
      For initializing: sem_init(&lock, 1)
      
      
      Fork/Join problem
      sem_t s;
      void *child(void *arg) {
        printf("child);
        // What goes here? - sem_post()
        return NULL;
      }
      int main(int argc, char *argv[]) {
        pthread_t p;
        printf("P: Begin");
        sem_init(&s, /*What goes here?*/ - 0);
        Pthread_create(&p, 0, child, 0);
        // What goes here? - sem_wait()
        printf("P: End");
        return 0;
      }
      
      
      reader/writer locks
      - many readers can be in a critical section at once or one writer
      typedef struct _rwlock_t {
        sem_t write_lock;
        sem_t lock;
        sem_t readers;
      } rwlock_t;
      void rw_init(rwlock_t *L) {
        L->readers = 0;
        sem_init(&L->lock, 1);
        sem_init(&L->write_lock, 1);
      }
      void acquire_rlock(rwlock_t *L) {
        sem_wait(&L->Lock);
        L->readers++;
        if (L->readers == 1)
          sem_wait(&L->write_lock);
        sem_post(&L->Lock);
      }
      void release_rlock(rwlock_t *L) {
        sem_wait(&L->Lock);
        L->readers--;
        if (L->readers == 0)
          sem_post(&L->write_lock);
        sem_post(&L->Lock);
      }
      void acquire_wlock(rwlock_t *L) {
        sem_wait(&L->write_lock);
      }
      void release_wlock(rwlock_t *L) {
        sem_post(&L->write_lock);
      }
      => Problem with this lock: [writer] starvation
      





      
          
