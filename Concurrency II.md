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

So far, we've been using hardware primitives as xchg, fetch_and_add and so should there be OS support? The scheduler might want to know whether a process is spinning endlessly: yield() system call - takes a running thread calls yield which changes the state of a thread from RUNNING => RUNNABLE (then gives up the CPU)



