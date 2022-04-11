# Virtualization
The most fundamental abstraction that the OS provides (Physical, one/few -> Virtual, many). The CPU and Memory are fundamentally affected by this abstraction.

Virtualization is an **illusion**: Each running program thinks it has its own CPU and private memory.
## Key aspects of virualization
- **Efficieny**: lower (over heads might occur)
- **Security**: restricting when user programs run (shouldn't read the entire hard drive rather should only be able to access its own address space). As an example, one of the reasons for the bug in intel processes was that, through this timing based channel it was allowing a process to arbitrarily read all of memory (security hole)

**CPU Virtualization**: **time sharing** (basic principle behind this abstraction) - When we want to share the OS, we give one process a little bit of time then give the other process a little bit of time in using the CPU in turn.

**OS in general manages resources**: the CPU, the memory

**Space sharing**: its like splitting the memory and giving it to differnt processes. These two concepts time sharing and space sharing are sometimes known as multi-programming.

## Abstraction of a virtualized CPU
**Process**: running program (program - lifeless binary executable that resides on a disk(instructions and data), when executed (runtime stack, heap, program counter))

**Components of a process**

- The process's memory (private): "address space" - in there resides code, stack, heap and static variables
- **Registers**: 
      - are changing constantly when a program is running
      - **PC / Program Counter**, **SP / Stack Pointer**, **GP / General Purpose Resgisters** - like eax, space for programs to perform their computations
      - basically used for performance (on the hardware, the processor should access data that is used for computation and compute on it very fast and comparing to register access memory access is very slow)
      - explicit form of caching - bring values in for the processor to work on it, since going to the memory would make it slow
      - only one set of these registers - are shared among processes by switching back and forth while being careful with their contents
- I/O State: opened files relevant to the running program

At the low level there are **Mechanisms** (how things work at a fundamental level; as an example for building an OS that uses time sharing for multi-programming, we need mechanisms for switching the processes(OS Scheduler)). On top of these are **Policies** - an algorithm for deciding how to best utilize the resources given the processes.

### Limited Direct Execution
Core mechanism behind CPU and Memory virtualization

**Direct Execution**: in-terms of efficiency; CPUs are very fast, mostly run directly on the CPU/hardware (if not careful, the program can do anything it wants)


**Limited**: in-terms of security; sandbox an environment where the running program can not do anything it wants

Lets consider this **Protocol - Direct Execution (not the limited version)**: 

- OS, the first program that runs on the machine
- To run a certain program "A", the OS first loads the prgram on memory (**loading**) and now the memory will contain the code, heap and stack (initially arguments such as argv and argc are put on the stack) of the program loaded, then within the CPU the program counter is set to point to the code within the memory and the stack pointer to the stack again within the memory.

From a timeline perspective:

------------------------------------------

      OS - running
                  | jumped
                  "A" - running -------
                  
------------------------------------------

Problems with this: 

- "A", the user process what if it wants to do something that is restricted? (like accessing a disk, creating a process itself, acquiring more memory for itself)
    - Problematic: You can read read everything on the disk, memory; where the secrecy of the machine is no longer maintained; would lose control of the machine
    - How to allow processes to do restricted things but control it?
- What if the OS wants to stop "A" and run another process lets say "B" 
    - When the process "A" is running, the OS is not running so how is the OS going to make a decision?
- How does the OS regain control of the CPU? 
    -  What if "A" does something that is slow (like disk I/O)?


### To allow user processes to do restriced things but control it
Requires machinery from the hardware and interaction with the OS

**Mode**: Per CPU bit

- Part of a register on each CPU
- Tells what kind of program is executing on the CPU at that time
- OS - "Kernel mode", the OS can do anything
- User Program - "User mode", can only perform a limited number of things, the hardware is going to prevent the program from doing certain types of things while in this mode. So, we have this bit in the hardware, indicating the mode of the running program. 
How to get into these modes?
How to transition?
    
    
To do these we're going to do something at **boot time**:

- logically, when the system starts up we're going to **boot in kernel mode** **(If some malicious code is injected on the OS or the OS is replaced by some other binary similar to the OS, its going to have full control over the machine thus making it problematic since its running in kernel mode)**
- When we want to run a user program:
      - We need **special instructions** that both **1,** transitions into user mode **2,** jumps to some particular location in the user program
- When the user program wants to do something restricted (like disk I/O): we're going to change from user mode -> kernel mode and change to protected mode
      - **1,** transitioning to kernel mode **2,** jumps into kernel (but is a restricted jump) - we don't want to allow a user program to jump to an arbitrary location while in this mode

Two instructions that come in pair are used for the switching of modes

- **trap**
    - Powerful, system support provided by the hardware to allow us to build OS like features
    - allows us to make a restricted jump into kernel and to elevate "privilege" (user -> kernel)
    - makes sure to save enough register state so that we can return properly (not to lose track of the state of the process when we switch to kernel mode)
- **return from trap**

--------------------------------------------------------------------------------

      "A" - running              ---------------------- running   (User Mode)
                   | trap        | return from trap
                   OS running ---                                 (Kernel Mode)
                   
--------------------------------------------------------------------------------

Traps offered by the OS are sometimes called **system calls** (set of services provided by the OS to user programs like opening a file, asking for more memory, create another process)

How does the OS restrict where to jump to?

- Happens at **boot time**:
      - OS starts up in kernel mode
      - Tells the hardware where to jump to when someone issues a particular kind of trap (which is usually done by specifying the trap number)
      - In the OS, set up **trap handlers** (issuing another special instruction: tell the hardware where the trap handlers reside in the OS memory)

We need to save (often done by the hardware) and restore the **register state** of the process.

### How does the OS regain control?
-------------------------------------------

      OS - running
                  | 
                  "A" - running -------
                  
-------------------------------------------

But the OS isn't running on the CPU anymore; programs that don't run can't make decisions.

**Cooperative approach**: Hope "A" doesn't do bad stuff

Reboot - to reset the system; cause the OS didn't have a way to regain control

**Non-cooperative / Preemptive approach**: In all modern OSes we prefer this. It is based on the hardware support - **timer interrupt**

At boot time: 

- OS - in kernel mode
- installs trap handlers
- starts the interrupt timer: after a few milliseconds its going to interrupt the CPU between some instructions
      
-------------------------------------------------------------------

      OS - running                   OS: timer interrupt handler
                  |                 |
                  "A" - running -----
                  
-------------------------------------------------------------------


**Side note**:

- **Traps**: explicit instruction that happens (like trapping into the OS)
- **Interrupts**: externral events that happen between instructions

In X86 the trap instruction is called int - for interrupt (now thats confusing)

