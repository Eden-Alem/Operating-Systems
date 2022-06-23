## Concurrency
- **Concurrency Bugs**: types
  - Atomicity Violations (data races): need locks to be solved
  - Ordering Violations (control races): need condition variables to be solved, assume ordering: event1 before event2
  - **Deadlock**: breaking the cycle to be solved

        In code: locks L1, L2; threads T1, T2
        
              T1                       T2
        acquire lock(L1)        acquire lock(L2)
        acquire lock(L2)        acquire lock(L1)
        
        Let's consider the situation where;
        T1 aquires L1
        Interrupted
        T2 acquires L2
        T2 wants to acquire L1 (already held)
        T1 wants to acquire L2 (already held)
        
        A cycle happens! Both threads have hold on to a resource where both are waiting for the resources held to be released (stuck waiting).
        
- Why deadlock occurs?
  - Cause software has complex dependencies

                      lock1  lock2
        Example:        |      |
        void function (obj1, obj2) {
          // perform computation over both objects atomically
          obj1.lock()
          obj2.lock()
            .
            .
          // release locks here  
        }
        
        If we called our function passing object A and B on two separate threads as follows:
        T1 - function(A, B)
        T2 - function(B, A)
        It will set up a possibility for a deadlock!
   
   
- Conditions for deadlock: For deadlock to occur all four of these conditions must hold
  - mutual exclusion: resource that needs exclusive access (shared data structure), another request is delayed
  - hold_and_wait: thread holding a resource and waiting for some other resource
  - no preemption: holding a resource and can't be taken away
  - circular wait: T1....Tn, cycle exists amongst the threads


- **Goal: Prevent deadlock** (by breaking just one of the above conditions)
  - Prevent circular wait: 
    - using metalocks: for eg; f(A,B): acquires metalock (acquires A lock, acquires B lock) releases metalock. The acquiring of the locks in the middle happens atomically thus the ordering doesn't matter anymore.
    - define a lock ordering (most common solution)
  - Prevent hold_and_wait:
    - lock acquisition lock (metalock): hold metalock while acquiring (makes lock acquisition atomic except for the metalock thus we're not stuck holding a resource and waiting for another)
  - Prevent the needing of locks (mutual exclusion environment)
    - "lock free" concurrent programming: idea - how to build concurrent data structures with out locks? (use powerful low-level instructions).    
    
    
          Example 1; critical section (where counter += 1), a power instruction (fetch_and_add(counter, 1) - done atomically on the hardware). 
           
          Example 2(list insertion):  
          void list_insert(int val) { 
            node_t *n = malloc(); - thread safe
            n->val = val; - private value that got passed in on the stack; not a problem
            (n->next = head; 
            head = n;) // needs to happen atomically: use CAS 
          }, 
          hardware instruction - compare_and_swap: 
          int CAS(unsigned *addr, unsigned expected, unsigned val) { 
          if (*addr == expected): 
            *addr = val; 
            return 1;//success 
          return 0;//failure 
          }
          
          list insertion using CAS for swapping in the new head in the list:
          void list_insert(int val) { 
            node_t *n = malloc(sizeof(node_t)); 
            n->val = val;
            do {
              node_t *old_head = head;
              n->next = head;
            } while (CAS(&head, old_head, n)==0);
          }
          In this case there is a potential starvation problem (livelock - threads are running never waiting and when they enter CAS they fail), contention
          Solution: If fail: back off (wait for some time (exponentially increasing time each time it fails) and try again)
          
  - Prevent no preemption:
    - pthread_mutex_lock(), can't preempt
    - other primitives: pthread_mutex_trylock(), either succeeds and grabs lock or fails if lock held already. 

          top:
            lock(L1)
            if (tyrlock(L2) == FAIL) {
              unlock(L1);
              goto top; // wait some time
            }
            
We can also **avoid** deadlock: based on **scheduling**. T1: L1,L2; T2: L1,L2; scheduler: run T1 to completion, then run T2 (never run threads that could potentially deadlock at the same time) - used in the case where we know which threads make use of which locks (detailed info), not generally useful cause we're not aware of which threads have which locks etc.


**Detect/Recover**: used in complex distributed databases - where locking patterns are so complex. Easier to detect when deadlock happens and kill something to revive it back with sufficient machinery to deal with that.
          
          



