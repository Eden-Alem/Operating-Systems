## Background: CPU
**Memory:** Program is loaded here, data resides here

**CPU:** execute instructions and manipulate data

Abstract model of the CPU when running a single program:
- **Fetch** (PC)
- **Decode**: Figure out which instruction it is
- **Execute**

- But when building an OS, we're going to need a different model of the CPU (more things will be going on to accomodate what we want from the system a protected, efficient and controlled system).
- We have modes (**user, kernel**):
      - Before execute: check permission (is this okay to execute?)
      - If *not OK*: the hardware will detect it and do something (as an example, segmentation fault; accessing memory illegally). In General, the hardware will raise an exception of some kind. That is when the OS gets involved (Exception handler runs; usually by large the natural reflection is that if the process is doing bad, it gets killed immediately)
- **Processing of interrupt(s)**: Between instructions external events are happening outside of our program (like hardware finishing a request, key pressed); outside the normal flow of execution. An interrupt(s) might happen and processor checks: if it happens, pending interrupts are handled (OS gets involved).

Support for the OS: **System support**: ability to limit particular instructions from executing and handling of interrupts

## CPU Virtualization Mechanisms
OS: implement **Limited Direct Execution**
- We want the hardware to do the checking since its much faster. We can't always go and check if an instruction to execute is okay by always going to the OS since its software, checking on software is much slower. For important times during exceptions, we ask the OS to take control.

At Boot time: OS runs first (in privileged mode)
- Installs **handlers**(functions(code)): telling the hardware what code to run on exception/interrupt; also explicit traps from the program asking to service a system call. 
      - The installation is done by a privileged instruction.
      - For example, for X86: lidt (load interrupt descriptor table): If you write this on a user program (installing your own trap handlers); the hardware will try to execute that instruction and then check the mode and that would result in a no and an exception would be raised likely killing the process.
- Install **timer interrupt** (privileged instruction)
- Read to run user programs

From a timeline perspective:

--------------------------------------------------------------------------------------

    OS -------                  ----------------                        (Kernel Mode)

             | ret-from-trap    | trap          | ret-from-trap

              "A" --------------                ------------------->    (User Mode)
              
---------------------------------------------------------------------------------------

Types of interactions we can have with the OS:
-  1) "A" wants OS service (system call): reading; writing; exec, fork in UNIX; create_process in Windows; exit; wait; network I/O
        - issue special instructions: **trap** instruction (in X86 called "int"). We make use of traps for the following:
                - For transition (User -> Kernel)
                - To jump into the OS: target - trap handlers
                - Save register state (to enable resuming execution later)
        - "Jumping to the kernel is more expensive"
        - OS system call handler runs
        - ret-from-trap is then issued (which is the opposite of trap) 

To run the first user program, the OS uses the instruction **ret-from-trap** (as indicated in the above timeline diagram); it sets it up so that it starts from your program's entry point (in C "main")

**Timer interrupt**: way to regain control of the CPU (by OS)
- OS handler runs: it can switch to a different process
        - **Context switch**: inside kernel we switch from one stack to another in the context of a different process.


**OS**: Must track different user processes in the system.
- **Process list** (main a certain data structure of the processes; useful for our policies) 
- Per-process information: PID (process identifier), files it has open
      - **"State"** (of the process): READY, RUNNING, BLOCKED (on I/O)


### Problem #3: Slow Operations (like I/O)
**CPU**: processes billions of operations per second

**Harddrive**: processes thousands of operations per second (The reason why SSDs came into play; they process hundreds of thousands of operations per second)

-----------------------------------------------------------------------------
      
      "B"                                       ---------------------->
                                                |
      "A"---------                              | ret-from-trap
                  | trap (sys. call)            | 
       OS         ------ -----------------------
                        | issues I/O
                        | to disk
       disk             ------------------------> (SLOW)

------------------------------------------------------------------------------

Allows better utilization of the CPU (overlap, in a concurrent system)

=> **Remark:** There are times in which waiting and not doing something makes the system more efficient, when is it? (wait causing you to do less work and is more efficient). Will look into it in future notes :)

Side note: Back in the old days OS support was also termed as "Master Control Program"

**States**

-------------------------------------------------------------------------------------------------------------

                             ------------------------ RUNNING <-------------
                             | (Issued                      |              | (Scheduled)
                             |  I/O)          (descheduled) |              |
               BLOCKED <------                              -------------> READY <---------
                    |                                                                     |
                    -----------------------------------------------------------------------
                                                (I/O done)
                                                
--------------------------------------------------------------------------------------------------------------

**Zombie state**: When a process is dead but not yet cleaned up.

OS: tracks all these things with the goal of **efficieny** in mind.

**Summary**:  General protocol - Limited Direct Execution; core of Oses, sandbox environment created for user processes.

## Mechanisms -> Policies
OS: Policy - **The what?** (What do we have to know to make the decision of running which process intelligently? (like run A or B))

The Mechanism - **How?**

**Assumptions**: (each process can also be termed as **Job**).......some of these are for sure unrealistic (for the sake of simplicity)
- Set of jobs; all arrive into the system (created) at the same time
- Each job only uses CPU (not worried about I/O for now)
- Each job runs for the same amount of time (Ti is the same for all processes); known ahead of time
- **Metric** (quantitative measure): turnaround time (time job ended(completed) - time arrived in the system)
- Zero cost of context switching mechanism (there was an additional implicit cost to this - hardware is fast at doing the same things over and over but by switching back and forth its going to be slower at first then will gradually get faster) 

**Side note**: Paranoid mindset - "Think like an adversary to build a rhobust system"

As an example for taking our above listed assumptions into consideration; let's take processes A,B,C in which all arrive at t=0 and with a runtime of 100 time units

For scheduling them:
- **Algorithms**: FIFO (First In First Out), one of the basic algorithms
      - Let's consider alphabetical order since they all arrived at the same time (A, then B, then C) - first, run to completion

--------------------------------------------------------------------

      "A" ------------- "B" ------------- "C" ------------- 
      |0                |100              |200             |300

--------------------------------------------------------------------

Turnaround time (time job ended(completed) - time arrived in the system; T)

TA = 100 - 0 = 100

TB = 200 - 0 = 200

TC = 300 -0 = 300

Tavg = (300 + 200 + 100)/3 = 200

**Remark**: It's very hard to know anything about a program's runtime. We'll see in future notes how this is handled by the OS :)

Now let's relax some of our assumptions listed above and build something realistic:

**Relax**: (All jobs have the same runtime) X

A: 100, B: 10, C: 10

----------------------------------------------------------------------------------------------

      "A" ---------------------------------------- "B" ------------- "C" ------------- 
      |0                                            |100              |110            |120

-----------------------------------------------------------------------------------------------

Tavg = 110; FIFO performs poorly here let's rather consider processing the processes with smaller runtimes first

-------------------------------------------------------------------------------------------------

      "B" ------------- "C" ------------- "A" ---------------------------------------- 
      |0                |10               |20                                         |120

-------------------------------------------------------------------------------------------------

Tavg = 50; much more responsive (quicker system)

**Goal**: Schedule Shortest Job First (SJF) - given the assumptions that all processes arrived at the same time; it gives the lowest average turnaround time.

**Relax**: (All arrive at the same time) X

-----------------------------------------------------------------------------------------------------

      | (A arrived)                                  (short run-time)               
      "A" ---------------------------------------- "B" -------------        (Not that responsive)    
      |0        |             (waits)               |100            |110            
                | (B arrived)

------------------------------------------------------------------------------------------------------

Generalization: taking into account that jobs arrive at different times; Shortest Time to Completion First (STCF)
- Compare what time is left to the time of the new job that has just arrived
- New thing we're doing here: **Preempt** job (in some cases, we're stopping an existing process and starting a new one)

**Introduction of a new metric (response time)** 

Some define this metric as the time until a process generates a "response" of some kind.

And we define this metric as how much the time the process takes before it starts running (time first runs - time arrives)

**Policy to minimize response time?**
- Round Robin Scheduling
      - length of time is called quantum/time slice (multiple of our timer interrupt period)
      - Trade off:
            - shorter time slices: better response time but high context switching overheads
            - longer time slices: worse response time but more efficient

**Idea so far:** We want to do SJF (STCF) but we don't know job lengths, and we also like to take response time into consideration (not just turnaround time). How to build a real scheduler that takes into account all these things? :)


**Side note:** Qemu (the emulator) is a program that boots like a real x86 system and emulates it in software which makes it basially slow.



**Policies:** Deciding inside the OS Scheduler; at a given moment which process should run?

-> Shortest Job First (SJF): The problem here is that  it assumes the knowledge of run time (we don't know how long processes will run for)-there are some environments where that this might not be true

-> Round Robin: Was frequently switching between jobs. This relatively is responsive (for interactive jobs). The problem here is that it stretches all the processes time.

### Develop a real scheduler policy: Classic Unix Scheduler
**OS Scheduling Policy - Multi-level Feedback Queue(MLFQ):** 

**Problem:** Don't know very much about the running processes (jobs)
          
Like to learn: which are short jobs, longer running jobs? How?
          
