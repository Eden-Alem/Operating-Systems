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

**Process API:** included on an interface of an operating system
- Create: An OS includes a method to create a new process (run a program you have indicated)
- Destory: A process will exit after finishing execution but a user might prefer to halt or kill a process thus an interface to halt a runaway process is useful
- Wait: Sometimes its useful to wait for a process to finish thus a waiting interface is provided
- Miscellaneous Control: Sometimes other controls of a process are possible by some OSs like suspending a process (stop it from running for a while and resume it)
- Status: interfaces to retrieve status information (how long the process has been running, state the process is in)

**Process Creation**

For the OS to run a program; it loads its code and any static data (local variables) on to memory, in the address space of the process. Initially, programs reside in disks (in some modern systems flash-based SSDs) so the OS is required to read the bytes from the disk (stored in some executable format) and place them in memory.

In early (simple) OSes, the loading process is done **eagerly** while modern OSes load programs **lazily** as in when the program is only needed for execution does it get loaded on memory. For lazy loading it makes use of paging and swapping.

Some memory must be allocated for the program's runtime stack (stack); the OS allocates this and gives it to the process (the C program uses the stack for function parameters, local variables and return addresses). The OS will usually also initialize the stack with arguments (argv, argc). The OS may also allocate memory for the program's heap (In C heap is used for explicitly requested dynamically allocated data; malloc and free are used). The heap is used for data structures such as linked lists, hash maps and trees. Initially the space allocated for a heap is small but as the process requests for more memory allocation using malloc the OS gets involved and allocates additional memory for the process.

In UNIX systems, each process by default has three open file descriptors (input, output and error) for a program's ease of reading from a terminal and printing out to a screen.

The OS's next task is to start the program running at its entry point (main()); through a specialized mechanism it jumps to main and transfers control of the CPU to the newly created process. Thus program execution begins!

**Process States:**
- Running: a process is running on a processor, its executing instructions
- Ready: a process that is ready to run but for some reason not chosen to do so yet by the OS
- Blocked: a process has performed some kind of operation and is not ready to run until a certain event takes place

At the discretion of the OS; when a process moves from a ready to a running state its referred to as **scheduled** and when from a running to a ready state its referred to as **descheduled**

**Image over here**

**Datas Structures**

The OS is a program and includes some key data structures to keep track of data (**process list/ task list** is one of such structure to track ready and running, blocked processes). Type of information tracked for each process in the xv6 kernel (**register context** - register contents for stopped processes; when a process is stopped its registers are saved to this memory location and the OS can resume running a process by resoring the registers: **context switch**)

The individual structure that stores information about a process as a **Process Control Block(PCB)/process descriptor**

- initial state: the state the process is in when created
- final state: a process has exited but not yet been cleaned up (In UNIX systems, known as the zombie state)

