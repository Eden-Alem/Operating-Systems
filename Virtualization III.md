### CPU Scheduling Policy
**Policies:** Deciding inside the OS Scheduler; at a given moment which process should run?

-> Shortest Job First (SJF): The problem here is that  it assumes the knowledge of run time (we don't know how long processes will run for)-there are some environments where that this might not be true

-> Round Robin: Was frequently switching between jobs. This relatively is responsive (for interactive jobs). The problem here is that it stretches all the processes time.

### Develop a real scheduler policy: Classic Unix Scheduler
**OS Scheduling Policy - Multi-level Feedback Queue(MLFQ):** 
- **Problem:** Don't know very much about the running processes (jobs)
- Like to learn: which are short jobs, longer running jobs? How?
  - Measure: using the past to predict the future
- Many Queues: Job is on one queue at any given time (but might change over time)

                        Priority
      Q2    A           highest
      Q1    B, C        middle
      Q0    D           low
      
      
- Each queue has a notion of priority
- Rules:
  - If the priority of "A" > "B": "A" runs (implicitly "B" doesn't)
  - If the priority of "A" == "B": Round robin between them
  - Start: Highest priority
  - If the process uses the time slice at a given priority, then at the end of the time slice move down one level of the queue

Example: (One job)

      Q2    A---->
      Q1          | A--->
      Q0                 | A---------------------............
            ------------------------------------------------>
                                   (time)

Example: Long-running job : "A", Short-running job : "C1", "C2"..... "Cn" (Cs that come in periodically)

      Q2    A             | C1 |       | C2 |   (benefit of a short job getting quick response time)
      Q1     | A          |    |       |    |
      Q0         | AAAAAA |    | AAAAA |    | AAAAA......  (long-running jobs end up at a low priority)
      
 
- *Adversary thought*: Let's say we create a large number of short jobs(never ending, everytime a short job ends a new one is created)
  - What happens to "A" (long running)?  **Starvation**

How to ensure long running jobs make progress?      
  - General idea:  long running jobs need to move up in the priority
  - Rule: Every **T(reset interval)** seconds, move all jobs to the highest priority
    - The nature of the job might change
    - If the job changes between modes of **interactive** (need the CPU cause its expecting the user to do something) and **batch** style computation (long running, compute a bunch of stuffs in the background)
    - The policy with this new rule allows the scheduler to re-learn about the job periodically
 
Another Concern (I/O):

                                    A: long running (batch) job
                                    B: I/O job
      Q2    A             |B(issues an I/O)  |B| ...... 
      Q1      | A         | |                |
      Q0          | AA..A | | AAAA.........A |  
      
      
*Naive* rule for I/O: If the job runs for less than the timeslice, we just stay at the same level (like "B" in the above diagram, interactive brusts which we want to relatively run quickly, Shell is a good example)

Problem: **Gaming** the scheduler
      
      B----   B----  (Give up the CPU before checked and will be kept at the same queue/level)
            A 
So we can take over the machine if not careful with this very basic saying "If you don't use your timeslice you can stay at the very same level"

Modified version: Better accounting - If a job uses up a *quantum*, it moves down (doesn't matter if it uses it in little slices or one whole slice, we have to account everytime it uses the CPU how much it uses it for)

Parameters: MLFQ
- N Queues: # of queues in some systems could be set by the admin
- Q: Quantum/timeslice length (per queue); it increases as we move down the queues from high priority to low priority
- T: Reset period (interval); how often do we have to move jobs up from a low level to the highest level

How to configure?
- **Mystery**
- Old school: Usually what people do, use defaults
- New school: Machine learning

### Virtual Memory
*Process*: running program; abstraction of our computer (lets us have a CPU and a memory)

**Side note:** its even more challenging to build an effective multi-core scheduler (though we first need to know concurrency). Main scheduler of linux is CFS (Completely Fair Scheduler)

**States of a process**
- **Registers:** When we're virtualizing the CPU what we were mainly dealing with was registers (PC, general purpose registers; swapping register contents (saving and restoring) and switching between programs quickly).
- **Memory state:** abstractly contains code, heap, stack. Memory of each program **virtual address space**

**Goals** for each process is to create an illusion
  - large memory (if adress space gets full; will take part of the data structure stored in the address space and will manually place it in  the disk and can use the freed space but to access the moved part of the data we'll need to go and get it from the disk; **ease of use problem**)
  - private (protected) memory: we don't want processes to readd from eachother or out from the kernel; **meltdown bug** (flaw in processors where an untrusted user program can read all of the physical memory of a system, was easier to mitigate); **spector** (is complicated and have had some mitigations but still unsure how many problems like this exist)
  - efficient memory: Limited Direct Execution (regarding direct execution in memory terms when doing loads, stores or instruction fetches, we want the hardware to do it without OS involvement)

**Side Note:** The first computers I program back in the days; TRS-80 Model 1, physical RAM was 16KB 

#### Mechanisms
Combination of **hardware** that we need and **OS support**

**Memory Accesses:**
- instruction fetch
- explicit loads (Memory -> CPU), stores (CPU -> Memory)

Need to interpose on memory accesses (not just access any memory you want)
- General mechanism: **Address Translation**
  - byte addressable memory (smallest unit to generally interact with the memory is 1 byte at a time; fudamentally modern systems are byte addressable)
  - to change 1 bit in a byte addressable memory: For a storage system (like memory (byte addressable), harddrive (smallest unit of access: sector addressable (512 bytes))); **read** the whole byte out, **modify** the bit and **write** it back on memory. Generally when doing small modifications its inefficient (if we were to change an entire byte we would have just **write** to it by skipping read and modify but to modify a bit we have to do twice as much I/O by doing read and modify). When writing an efficient program on any of the storage system we need to have some understanding on the granularity of the storage system (do operations in larger chunks).
  - We have to place the virtual address spaces of each processes in physical memory.

**Address Translation**
- On every memory reference, we're going to translate the virtual memory address from the process into a physical memory address where that data resides.

##### Mechanism #1(ancient of super computers, not in use today but is coming to revival):
**Dynamic Relocation (Base/Bounds):** it adds two registers to each CPU that tells about how the relocation of address spaces in memory is happening. 
  - As an example, process 1 (P1) has an adress space (AS1) and we're going to assume its very small and we eventually want to build a large virtual address space which is hard. We also have process 2 (P2) with AS2 which is again assumed to be small. We're assuming them small so as to fit these virtual address spaces entirely in the physical memory. So what we're going to do is place these address spaces at different locations in the physical memory in entirety. 
  - The address line number is supposed to be translated to another number perhaps from a 0 to a 1000 before going in the physical memory. How? **Memory Management Unit(MMU)** is part of the processor that helps with address translation. 
  - We have **two registers per CPU**
    - **Base:** represents the address in physical memory where the address space of the currently running process starts (**is for translation**)
    - **Bounds:** gives us **protection** (check that we're within our address space)
  - When a virtual memory reference comes out of a running program its going to go into the MMU and the MMU generates a physical address as an output which goes to the memory (Virtual -> Physical; Converted using Base + Virtual (the base can be considered as an offset) and then we check if its within bounds).


**Every address in a user program is a virtual address.** Discovering pointers is just an illusion, you don't know where these pointers are pointing to, they live in physical memory in arbitrary locations determined by the OS. As a user programmer we only get to see virtual addresses and they constantly get translated by the MMU to their actual location that they live in.


Modern OSes:
- Allow sharing of memory between processes: Meaning P1 contains a chunk mapping into an address space where P2 at another location has mapped into its address space and what that means is that those chunks both actually refer to some part of the physical memory that's the same.
- **Problem: Dynamic relocation which is the basic way of building a MMU doesn't support sharing**


      CPU (abstractly):
            while (1) {
                  Fetch (PC)
                  Decode
                  Execute (limit, in OS) 
                             |
              Our limits on the MMU side: Bounds
                                 |
                           if ok: go ahead
                           if not ok: raises exception                   
            }


**Side note:** Pipelining in a processor is basially as follows
      
      -> [] [] [] .....
         [] [] ......
         [] ......
      
if you have partially executed some further instructions that should have never executed cause the first instruction in the sequence above is going to raise an exception (meaning we have changed the state of the CPU a little bit with those later instructions that never should've executed). That problem had been sitting in CPUs dormant for 30 years as a huge **security vulnerability** (related with meltdown bug)


When not ok:
- Hardware raises exceptions (illegal memory access):
  - Hardware notifies OS: process had bad memory acess 
  - @boot: OS set up exception handlers
  - OS will generally: Kill process


Base/Bounds:
- Advantages
  - Fast
  - Simple
- Disadvantages
  - hard to allocate memory (have to find a large contiguous region)
  - hard to expand; not flexible
  - hard to share memory (good eg of using shared memory: code)
  - defragmentation (high cost of moving for allocation)


#### Virtual Memory
Process; with the abstraction of building a large, private and efficient memory

Simple mechanism: **Dynamic relocation**

      P1 [] - size1           Physical Memory (where everything lives)
         [] -------------           [OS..]
                         |          []     (P1 and P2 are just abstractions)
      P2 []               --------> [] 100     (In reality we're placing these addresses somewhere in memory)
         [] - size2                 []          
         [] ----------------------> [] 1000
                                    []
                                    []


And dynamic relocation (base and bounds) gives us a simple CPU piece of hardware to translate addresses (virtual -> physical). Programs do explicit loads and stores (MOV instruction in x86, from memory into a register or vice versa) and all of those addresses everytime need to get translated. 
- **Base** tells the location of the address space in physical memory. - - **Translation:** offset(Virtual Address) + Base
- **Bounds**: Protection; when process 1 generates a reference, it can't generate one outside of its boundaries

**Side note:** When memory gets chopped off and we can't allocate the whole chunk in the physical memory its known as **Fragmentation**

**OS roles:**
- At the time of a context switch:

      P1   running   |
                     |
                     |  P2    running
                     |
           timer interrupt goes off
               [context switch]
               
- The OS needs to be cognitive and switch the contents of the base and bounds registers from the previously running process to the now running process (will save and restore based and bounds).
  - How to do so? With instructions: are **privileged**
- If the OS wants to defragment the memory:
  - Goal: the ideal memory allocator has everything packed in one side of the region your managing leaving the other side free
- Steps the OS would take to defragment?
  - Stop the processes (BLOCKED)
  - Way to decide where to put each (algorithm)
  - Careful to not do a non-destructive copy (of the address spaces to their locations)
  - Change base to the new values
  - Make p1, p2 ....(the processes) runnable 
- When might the OS change the bounds register?
  - To give more memory. If a user program calls **malloc**, that might have to ask the kernel for more memory (using a system call).
  - Then the OS in response: changes the bounds register
  - Somewhere between the deallocation of a died process (freed up memory) and allocation of that space to another process: the OS makes sure to zero out that space (after zeroing memory)
 

**Side note:** One problem with the meltdown bug was that the Linux kernel genrally had all physical memory mapped into the kernel virtual space and then that kernel virtual space is mapped into every user process and is generally not accessible but it turns out cause of this processor bug you could access that in memory.

**Every access is virtual**

Lack of flexibility:
- ideal address space is large

       (common organization of placing the stack and heap at opposite ends why? Its so that they can grow)
                  code  - 0
                  heap
                   |          


                   |
                  stack - max-1       (this doesn't work well with dynamic relocation why? There's a lot of empty space in the middle, which is a waste)  -> But, we'll assume this layout throughout the notes
                  
         How to organize the above if you have dynamic relocation?
                  code
                  stack (fixed size stack; if it overflows stack it'll start overwriting and the program will crash)
                  heap
                    |
                    
         Make a less flexible address space layout         


**Generalize base/bounds:**
- **Segmentation:** Multiple base/bounds registers per process, one base/bound pair per logical region of the address space (allows us to relocate pieces of the address space). The internal space (waste, internal fragmentation) between the heap and stack that isn't used is not mapped to physical address (rather we're allocating the pieces that we're using separately)
  - Do you have to still defragment memory? (from the OS perspective) Yes, there might be defragmentation
  - The OS roles are similar:
    - save and restore of base/bound pairs
    - gets involved in malloc
    - defragmentation
  - Advantage of being a little more flexible 
- **Stack smashing:** is caused by any time you inter-leave control and data on the stack (if we're interleaving return addresses on the stack, we can find a way with a pointer to override the stack)

**Detail on segmentation:** Which segment does this address (VA) reference refer to?

