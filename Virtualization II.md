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










          
          
          
          
