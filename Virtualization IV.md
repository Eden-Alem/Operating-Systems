## Virtual Memory
- **Mechanisms:**
  - Dynamic Relocation (simple memory systems; base/bounds): mapped from virtual address space(illusion) to physical memory
  - Segmentation: Generalization of base/bounds. Virtual address space (maybe containing code, heap, stack); independent relocation of each segment. 
    - Allows more flexibility (supports sparse address space).
    - Problems: 
      - support spaseness but is limited (same problem with dynamic relocation but at a slightly smaller scale than we have with the whole address space; where if top and bottom sections of the heap is allocated and if the space in between is freed the whole heap (segment) is still going to be relocated)
      - external fragmentation: we have to reject request sometimes or we have to compact memory (which is expensive; we have to move used memory space on the other end and leave the unused space on the other end)

- One of the roles of the OS is to manage physical memory.
  - 2 levels of memory management
    - first level: inside the virutal address space (Whats being managed here of your running program? Who's managing it?)
      - In c: malloc/free (malloc library is responsible for managing the heap, if you request 9 bytes of memory it looks for 9 bytes of consecutive memory and respond with a pointer of that address) 
      - Stack: part of the compiler tool chain(runtime); managed by the language runtime (when we need space on the stack we grow the size when returning from a function the stack shrinks)
    - second level: OS is managing physical memory
      - Decides which process's code,heap and stack is located on the physical memory (How do I find free space? How do I allocate it? How do I share it among?)
      - When the heap wants to grow in size we ask the OS for space in physical memory then once we get that space then our malloc library can use that space and passes it back to you

**Side note:** Some of the earliest computer systems, the atlas which is developed in England in the 60s used the technique known as **paging**


- **Paging:** potential problems of paging is that if not careful its too slow, and takes up memory space (memory hog)
  - Divide up two things (Virtual address spaces and physical memory into **fixed size (typically 4KB) units called pages**. 
  - A typical 32 bit address space with 4KB pages gives a million of pages in address space (With calculation; 2^32/2^12 ~ 4billion/4000 ~ 2^20,"1million"; 2^10 - 1KB(thousand), 2^30 - 1GB(billion))
  - The virtual address space is divided into these virtual pages (of 4kB of size) numbered from 0-n and each virtual page is mapped onto the physical memory (which is again divided into multiple 4KB of space) and is named physical frames (container of each of the virtual pages and is also numbered from 0-n); translation of virtual page numbers to physical frame numbers
  - Problems:
    - A lot of information: the OS needs to track and maybe the hardware needs to use (we might have to track a million virtual pages of each process's virtual address space where it resides in physical memory)
