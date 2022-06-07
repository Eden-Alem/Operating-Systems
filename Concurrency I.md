# Concurrency
- Classic "process" (single-threaded process; there's only one computation inside of this process at a time): has a virtual address space (code, stack, heap), registers like PC(program counter),IP(instruction pointer), SP(stack pointer), general purpose registers physically exist as a single component in CPUs (are virtualized)
- OS could switch to other processes (save and restore registers, switch page tables)
- In many cases, we have many "activities" going on inside a single process at a time (multi-threaded process)
  - For example in a process with 2 threads: there is one virtual address space(code, heap, stack1, stack2; if we have n threads within a multi-threaded address space we have n stacks), 2 register contexts (PC, SP and GP each for thread1 & thread2)

        The OS can context switch between threads (just as it can with processes)
              thread1    thread2     thread1
        CPU: [///////] [\\\\\\\\\\] [///////
                      |            |
             -------------------------------->
                        (time)
                        
        Each thread has the illusion of its own CPU (virtualized) but memory(the address space) is shared between these threads (same code, heap, but different stacks)
                        
##### Context switch between threads vs. processes
- In both cases: we save and restore registers
- Only when switching between processes: we switch address spaces (change the page table base register, PTBR)

#### Why multi-threaded processes?
- Parallelism (to run a program faster)
  - For example, for running a computation (as increasing each array element by 1) of a big sized array and a machine with 4 CPUs:
    - We can create 4 threads
    - And give each thread a chunk of the array and do work in parallel
    - (This can be done with different processes but is more complicated as we have to figure out moving the data between different address spaces; threads give the advantage of shared memory by default)
- Overlap (Concurrent)
  - For example, a webserver:
    - Get request
    - Parse
    - Read data from disk (issues a single or multiple disk I/O)
    - Return the data   

          Problem here from a timeline perspective is as follows:
                     (slow; disk operation even SSD)
          1 2 3--------------10ms----------------------------> 4
          ------------------------------------------------------>
                                (time)
                                
          Instead we make use of a multi-threaded server (consider requests R1, R2, R3):
          R1: 123 -------------------->
                 | (the OS switches to a different request)
          R2:     123
                     |
          R3:         ......
          
          Allows to concurrently process many requests and overlap CPU activity (like parsing)
  
  
**Side Note:** Threads + Trust Model - Threads within the same process trust each other           
          
   
Sometimes when an OS exits(crashes) it results in a segfault; its called the **Kernel Panic**
          
Posix Thread Library (posix standard): is the generic compatibility layer created across different forms of UNIX

A thread enables multiple instantiation of function calls at the same time (executing on their own stacks)

The independent stack of threads get created at run-time(during execution); when each time we create a new thread (with pthread_create in C) a new stack is created in the same virtual address space of the process (of the already running thread) and attached to the newly created thread for the duration of its existence.

        Lets consider an example illustrating race condition
        
        Critical Section: Named by Dijkstra for instructions in which if two or more threads enter this section at the same time could result in a problem
        The resulting outcome though if two or more threads enter this section is that a data race occurs and the outcome is indeterminate
        
        Let's consider the following three instructions in assembly language form (of x86 64bit) where balance is referring to the same memory address (known as instruction relative addressing) and is a global variable (meaning its shared)
        
        // Critical Section
        1: movl     balance, %eax
        2: addl     $1, %eax          balance = balance + 1
        3: movl     %eax, balance
        
        
         Thread1       Thread2       
         pc  eax       pc  eax      balance (in memory, shared)
         1  1000                    1000
         2  1001                    1000
         3  1001                    1001
                       1  1001      1001
                       2  1002      1001
                       3  1002      1002
                       
         The above works as expected (SUCCESS) :D
         
         
         Problem: arbitrary interleaving of threads   
         
         Thread1       Thread2       
         pc  eax       pc  eax      balance (in memory, shared)
         1  1000                    1000
                       1  1000      1000
                       2  1001      1000
                       3  1001      1001
         2  1001                    1001
         3  1001                    1001  // redundant store of 1001
         
         In the above case (could happen when an interrupt/external event occurs), an update is lost :(
         
         
         How do we solve this?
         Using locks; (in C pthread_mutex_locks) on the shared variable (in this case balance) before updating the variable (executing balance = balance + 1) we lock the variable and then unlock it; in which this guarantees that only one thread is in critical section at a time.





