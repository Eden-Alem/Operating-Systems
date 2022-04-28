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

