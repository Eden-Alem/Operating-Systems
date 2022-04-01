# Operating Systems
## Fundamentals of Computer Systems
Basic computer components: **CPU** (basic operation: fetch, decode and execute), **Memory** (Von Neumann Model of Computing: Everything including the instructions are in some store), Disk (being increasingly replaced by solid state storage, nowadays though both are still being used), mouse, keyboard, display, network

## What is OS?
- OS provides the abstraction of having as many virtual CPUs: For the purpose of **efficieny** (better utilization of resources/hardware) and **multi-tasking** (not limited by the physical hardware, ease of use)
- OS gives an illusion of a memory that is private to your program: For the purpose of **security** (not writing on someone else's memory; virtual memory)
- In UNIX file systems, write operations first buffer the values on memory for 30 seconds or so before flushing it to the hard drive/SSD (cause this operation is slow). But we can use the "fsync()" interface to force the operation on the disk.
- **trace** (can be created using a kernel tracing tool): shows how many functions inside the kernel were called during the execution of a program
- For file systems: how do we function in-terms of commiting data to the disk and reading data from the disk
- Threads: allow functions inside the same program to run at the same time (can be created using Pthread_create in C), its also known as multi-threaded or concurrent program (useful in running parallel programs and building web servers)

Accessing shared data results in different values everytime execution of a program occurs (due to race conditions - fundamental problem in concurrency). To solve this we can use locks (which makes the program slower - cost of correctness)

