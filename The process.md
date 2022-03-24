# Modern Operating Systems
## Virtualization

## The abstraction - the process
**Process** - a running program
The OS takes bytes (programs in the form of static data or instructions) and gets them running, turns them into something useful.

THE CRUX OF THE PROBLEM:
HOW TO PROVIDE THE ILLUSION OF MANY CPUS? Although there are only a few physical CPUs available, how can the OS provide the illusion of a nearly-endless supply of said CPUs?

- The OS creates the illusion by virtualizing the CPU (the illusion of each process owning a dedicated CPU while in fact only a few physical CPUs exist)
- Basic technique: **time sharing** of the CPU, as many processes can run concurrently putting performance at stake since sharing few CPUs among many processes slows each process down.

- For virtualization of the CPU: the OS makes use of the *low-level machinery and high-level intelligence.* Referring to the low-level machinery as mechanisms, its a method or protocol that implements a functionality. We can take context switch as an example (mechanism implemented by all modern OSs to stop a running program and run another one on a given CPU)
- On top of these mechanisms, the intelligence of the OS resides in the form of policies. Policies are used by the OS for decision making in which program to run while there are many programs waiting to be run (**scheduling policy**). And this decision is made by making use of the historical information, workload and performance metrics available.

*Separating policies and mechanisms is a form of modularity, software design principle*

The abstraction created by the OS of a running program is known as process. For understanding what a process constitues *machine state* is an important factor, which parts are important for the execution of the program.

One of the components of the machine state that comprises a process is known as **memory**; instructions reside in the memory. Data that is read or written while a program is in execution resides in **its address space**.

Other parts are **registers**; instructions explicitly read or update registers hence are an important factor in the execution of a program. Some particular special registers include the **program counter (instruction pointer)** (points on the instruction to be executed next), **stack pointer and frame pointer** (manage the stack for function parameters, local variables and return addresses)

Persistent storage devices could be accessed by processes too (I/O information such as open files).

*Time sharing - basic technique used by an OS to share a resource, allowing a resource to be used by one entity and then by another (Eg resource - CPU, network link). Space sharing - a resource divided (in space) for those who wish to use it (Eg- disk space)*

Process API: included on an interface of an operating system
- Create: An OS includes a method to create a new process (run a program you have indicated)
- Destory: A process will exit after finishing execution but a user might prefer to halt or kill a process thus an interface to halt a runaway process is useful
- Wait: Sometimes its useful to wait for a process to finish thus a waiting interface is provided
- Miscellaneous Control: Sometimes other controls of a process are possible by some OSs like suspending a process (stop it from running for a while and resume it)
- Status: interfaces to retrieve status information (how long the process has been running, state the process is in)

Process Creation

For the OS to run a program; it loads its code and any static data (local variables) on to memory, in the address space of the process. Initially, programs reside in disks (in some modern systems flash-based SSDs) so the OS is required to read the bytes from the disk (stored in some executable format) and place them in memory.

In early (simple) OSes, the loading process is done **eagerly** while modern OSes load programs **lazily** as in when the program is only needed for execution does it get loaded on memory. For lazy loading it makes use of paging and swapping.




