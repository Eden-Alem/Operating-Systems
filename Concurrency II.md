## Concurrency
- **Data races:** Eg; Shared counter (x+=1), list insert
- Solution: **Locks** - give a little control over what runs
- Lock: construction
  - Simple *spin lock*: The first time a thread is accessing a lock it'll return 0 and set it to 1 atomically (successfully acquired the lock). Across multiple threads, only one will be able to return 0 and acquire the lock. To release the lock, it just sets it back to 0.
  - Problems:
    - Too much spinning (performance problem): the best case for spin locks is when there is not too many threads as there is less performance overhead to acquire the lock. But as **contention (threads are constantly grabbing locks and contending with eachother)** increases problems arise
    - Fairness (starvation problem)

          Bad case (contention; interrupted in critical section):
                 interrupted
          Thread1:--L-|                              |--UL---
          Thread2:     -L (spins) -------------------|
                         runs for a time slice    interrupt
          
          Too much spinning; lets consider 100 threads (instead of 2):
          T1 (interrupted)
          T2--------T100 (each will run for a time slice, 10ms)
          
          All contending to the same lock, and the thread that has acquired it gets interrupted while in critical section waiting for: ~100(number of processes) * 10ms(length of time slice) = 1 second of compute time is wasted.
          
          Fairness:
          T1 ....... Tn (contending for lock)
          Ti (is spinning), will it ever acquire the lock? No guarantee
          
          Solving this with ticket lock (control over which thread might acquire the lock next):
          ticket = 0
          turn = 0
          acquire section (figure out my_turn):
            my_turn = fetchandadd()
          T1 -> acquire; my_turn = 0
          T2 -> acquire; my_turn = 1
          
          Spin while the locks turn != to its turn(my_turn)
          T1: its my_turn so it acquires the lock (0)
          T2: if it interrupts and runs, won't acquire the lock since T2's turn is 1 and lock->turn is 0 (not yet)
          
          In releasing the lock, it increments the lock->turn by 1
          T1: release; inc turn (allowing T2 to get the lock when it runs)
    
    
- Does the fact that the fixed size of an integer as we keep incrementing will overflow (wraps around the first integer value), a correctness problem for the ticket lock implementation? (Add-on in C, overflow is undefined; can do anything)
  - Yes, we need a unique identifier for each thread's turn (cause if two threads have equal turn values they will both enter the critical section causing our lock to be broken). But in the case of 32 bit integer size for this overflow to occur as T1 is running another 1billion threads need to be running as well which is highly unlikely (so, as long as the number of contending threads is less than the size of the ticket size we're all good :D)
          
          
In x86, **Atomic Exchange** in hardware; works in a way where its given the address of a lock, returns the old value and at the same time sets a new value to the address its pointing towards (Eg in C: xchg(&mutex->lock,1)==1; at the same time we're comparing it to 1 and setting it to 1)

An instruction named **fetch and add** is a powerful atomic instruction (included in some architectures) where it remembers the old address value increments the address value by 1 and returns the old value it remembered(stored) prior the incrementation of the address (single indivisible instruction).

- So far, we've been using hardware primitives as xchg, fetch_and_add and so should there be OS support? The scheduler might want to know whether a process is spinning endlessly; OS primitives:
  - Yield() system call - takes a running thread calls yield which changes the state of a thread from RUNNING => RUNNABLE (then gives up the CPU). Assuming 100 threads, the waiting time after the running thread is interrupted would be 100 * cost of context switch (much less than a time slice). 
  - Solaris (early multi-threaded UNIX OS): had an OS primitive to help build locks named **park()**(like yield but rather => BLOCKED not RUNNABLE) **unpark()**(takes a thread_id and makes thread_id RUNNABLE) 


**Race of control(how code is executing)**

    Fork/Join Problem:
    
    volatile int done = 0;
    
    void *child(void *arg) {
      printf("child\n");
      
      // something here to signal all done
      done = 1;
      
      return NULL;
    }
    
    int main(int argc, char *argv[]) {
      pthread_t p;
      Pthread_create(&p, NULL, child, NULL);
      
      // something here to wait for the child
      while (done == 0); // spin (inefficient; wastes CPU - uses it just spinning, shares the CPU 50-50 with the child eventhough its doing nothing)
      
      printf("parent: end\n");
      return 0;
    }
    
We want the parent to go to sleep (not using the CPU) and when the child is done to wake it up; for this we will use another primitive called the condition variable(CV).
    
A condition variable is a queue of waiting threads.
   
If we consider the condition variable as an abstract class it has two methods
- **wait:** puts the caller to sleep and releases the lock (atomically), assumes the lock is held when wait() is called, when its awoken it reacquires the lock before returning.
- **signal:** wakes a single waiting thread, if there is none waiting it returns without doing anything.
  
Usually, a CV is paired with some kind of a state variable (eg; integer, which indicates the state of the system).


    Fork/Join Problem(in another way); waiting problem - a main thread waiting for all the worker threads to finish their work (common design pattern of concurrent programs):
    
    pthread_cond_t c = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    volatile int done = 0;
    
    void *child(void *arg) {
      printf("child\n");
      
      // something here to signal all done
      Mutex_lock(&m);
      done = 1;   // Code b/n lock and unlock should be considered atomic
      Cond_signal(&c);
      Mutex_unlock(&m);
      
      return NULL;
    }
    
    int main(int argc, char *argv[]) {
      pthread_t p;
      Pthread_create(&p, NULL, child, NULL);
      
      // something here to wait for the child
      Mutex_lock(&m);
      while (done == 0) {
        Cond_wait(&c, &m); // release lock when going to sleep
      }
      Mutex_unlock(&m);
      
      printf("parent: end\n");
      return 0;
    }
    
    
    If we didn't use locks; let's consider the case where parent gets interrupted before calling cond_wait and the child does its work and does cond_signal (if there is no one waiting it'll just return ineffectively) and then the parent runs again in which it runs cond_wait instruction and enters BLOCKED state (waits) forever.
    
    
##### Bounded Buffer or Producer/Consumer Queue
- The situation occurs when we have one or more producer threads and one or more consumer threads.

      P1                  C1
      .       Queue       .
      . ->  [||||||||] -> .
      .         |         .
      Pn        |         Cm
                |
       Shared data structure
       
      Extra requirement: The queue is bounded (fixed size). Why?
        As an example, let's consider the UNIX pipe
                  grep hello file | wc
                     (producer)     (consumer)   
                The pipe is a shared queue
                
        And so if we let the producer run for long, it'll create a large amount of data and fills memory with it (thats why we need the queue to be bounded - not to waste memory).
        
      We can't use locks, cause it'll prevent a producer from producing when the queue is full. We make use of condition variables.
        






