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
  - The virtual address space is divided into these virtual pages (of 4kB of size) numbered from 0-n (virtual page number, VPN) and each virtual page is mapped onto the physical memory (which is again divided into multiple 4KB of space) and is named physical frames (container of each of the virtual pages and is also numbered from 0-n; physical frame number (PFN)); translation of virtual page numbers to physical frame numbers
  - Problems:
    - A lot of information (per process): the OS needs to track and the hardware  maybe needs to use (we might have to track a million virtual pages of each process's virtual address space where it resides in physical memory)
      - Has implications:
        - Generally its stored in memory (taking a lot of space and slowed access). The OS owns a page (generally multiple) within the physical memory where it keeps the translation information.
        

Example: Smalll "toy" virtual address space; page size: 8 bytes; VAS size: 32 bytes (5 bit address space)

    Virtual Page
    Numbers (VPN)
             0 [ 0 | 1 | 2 |      ] - each of these pages, contain a bunch of bytes in this case 8 bytes (and if each byte was labeled as follows); we can get to the bits by loading a byte and dealing with the bit
             1 [  |  |  |......|  ]
             2 [  |  |  |......|  ]
             3 [  |  |  |......|  ]
              Virtual address space 
                (VAS) with 4 pages

    Virtual address space and its constituent components:
       V.A.S [   |   |   |   |   ]
              ------- -----------
                VPN     offset
                 | address
                 | translate
     Physical Frame Number(PFN)  | offset - just gets appended, not translated
              
     Full binary address of each byte (0-7, bit level addresses):
         00000
         00001      - virtual addresses of the first page of bytes(0)
         00010      - on the same page, the top address bits aren't changing (first two, 00); infact it uniquely identifies the virtual page number(VPN); translation VPN -> PFN
         00011      - the remaining three bits of the address don't need to get translated (if we refer to the 4th byte it doesn't matter if we're talking about the virtual page or physical frame that its in, its the same 4rth byte) and we name that the offset
         00100
         00101
         00110
         00111
     
     
    if we have a 32 bytes of size VAS how many bits do we need in our VA? 
      - (How many bits do we need to refer to 32 different things? - 2^5). That means we need 5 bits.
    

- Translation Information: is stored in memory somewhere; where the OS is going to manage it and the hardware is going to use it
  - We call the data structure that stores this information a **page table**
    - Simple(to begin); an **array(linear page table)** (one page table per process)
    
          
          Contents (of page table)
              Virtual Page
              Numbers(VPNs)
                       0 [ 0 | 1 | 2 |      ] - 8bytes
                       1 [  |  |  |......|  ] - let's say we're interested in the virtual address 01101, 13 (valid one, resides in VPN-1)
                       2 [                  ] - not in use - exception/fault when trying to access
                       3 [  |  |  |......|  ]
                        Virtual address space 
                          (VAS) with 4 pages
                          
              Page Table: Array (Linear)
                  (one entry per VPN)
                  
          info that indicates whether its okay to access that part of the address space  
                  |
                valid  |  PFN
               [  1        1      ]         index by VPN
           --->[(translation info)] - page table entry(PTE)
          |    [  0               ]->invalid  |
          |    [  1        4      ]           |
          |                                   | inside of this entry resides the physical frame number
          |                                   |
          |   VA: [  |  |  |  |  ]            |
          |        ----- --------             |
          |         VPN   offset          [  | PFN ]      
          |          |                     contents   
           ----------


           VA: 01101     - infact bytes 8-15(which were in VPN1 actually reside in let's say as an example in physical frame 5, PFN5)
            VPN  01 | 101  (VA:13)
                 |(translated through the page table); take the VPN and index it into an array(VPN is not stored on the page table its just an index telling us which entry we're interested in and get the PFN)
            PFN 101 | 101  - physical address(PA); 45



##### How does translation occur?
- Limited(security) Direct Execution(hardware for efficiency) model

- Considering the page table, the hardware does most of the work 

              Physical Memory
           0 [    //////     ] - OS uses this, for the page table of P0
           1 [///////////////]
           2 [               ]
           3 [               ]
           4 [///////////////]
           5 [///////////////]
           6 [               ]
           7 [               ]
                                   
      While P0 is running the hardware needs to know a couple of information inorder to be able to do the translations and not have the OS involved. What does the hardware need to know?   
      - Location of the page table
      - Details of the system (like page size)
      - Structure of page table entry (In x86 as an example, every structure is defined by the architecture/hardware)

      Location of the page table: means there exists a per CPU register that holds the physical address of the page table of the currently running process, its called the Page Table Base Register (PTBR)
      
       Page table    -> in memory at address 4 (physical address)
       [ 1     1 ]    
       [ 1     5 ]    - Total of 4 bytes of size for the page table
       [ 0     - ]
       [ 1     4 ]
       -----------
         1 byte (size of a page table entry)
       
       Set PTBR to 4 (value changes upon context switching)
       
       When given a VA which we need to translate
       1. We need to figure out the physical address of the desired page table entry
               VA: 11 | 010   ->     PA: PTBR + (VPN * PTE)
                  VPN-3       ->          4   +  3   *  1  =  7
          
       2. Load the page table entry (PTE), goes to memory which is slow
       3. Do the translation by referring to the page table entry (VPN -> PFN)
                    11 | 010
                   100 | 010 - PA
       4. Load the byte of information (data)   

       Problems:        
        - we're paying an extra memory reference everytime (2, 4) which makes it really slow
        - Page tables are too big (million entries)
           32-bit VAS, 4KB page ~ million entries;
           if each entry is 4 bytes ~ 4MB page table
           if running thousand processes ~ 4GB of memory being used for page tables (huge amount of space)

       Side Note: Processors flip bits quickly (was figured out too quickly by the people who were building hardware using transistors that it flipped unimaginably too quickly) but going to memory takes a lot of time (perhaps hundreds of times longer)


- PTBR details:
  - privileged to update (restricted; not all can update)
  - Process table: per process information we're tracking like state, registers and PTBR (save/restore); the OS must do these as a data structure per process where its going to save the PTBR when it switches away from the process and then restore it on to the CPU of the running process


*Simple discussion*

        int sum = 0;
        int i;
        for (i = 0; i < 2048; i++){
          sum += a[i];
        }
        
        if we have an array of size a[....] and page size of 4KB; how many pages of memory are being accessed when running this code?
        
        In the array a we're accessing 2048 entries.
        How much data is being accessed? size of an int (4byte) * 2048 ~ 8KB        
        For accessing the array, how many pages are we accessing with a page size of 4KB?
        - No more than 3 since we don't know about alignment of the array [2, 3]. We could be lucky and access 2 pages fully or maybe half of the page could be accessed and the other half could be accessed from another page leading it to be 3
        
        [      ] - 4KB of page size    [       ]
        [//////]                       [-------]
        [//////]                       [///////]
        [      ]                       [-------]
        
        
        Where i and sum are in memory?
        - are somewhere on the stack
        - it could be on 1 or 2 pages
        
        The code?
        - load, store, comparison, jump (probably like a 5 instruction sequence)
        - we have 1 or 2 pages that are going to be accessed (assuming instructions are not too big)
        
        
**Side note:** *You think you have billions of cycles but efficiency still matters :)* To really write efficient code you have to understand what's happening at the hardware level, the OS level and the programming language compiler level.


Translation is too slow: it needs an extra memory reference (going to memory to get the page table)

        When doing translations, when doing a memory reference we refer to the PTBR to refer to where the page table is then we go to memory and get the PTE then do the translation and then we go to memory and get the requested data
        
        CPU                 Memory
        [         ]           [        ]
        [   PTBR  ]           [        ]
         |    | PTE(from memory) |   |
         |    --------------------   |
         -----------------------------
                     data
                     
        One of the most basic principles of building fast hardware is building hardware with caches. When frequently accessing something we can cache that in the CPU and in this case by storing a copy of some part of the page table we can make translation faster.
        
        The cache in the CPU is called the Translation Lookaside Buffer(TLB). Simply its an "Address Translation Cache". A cache in general is a smaller, faster memory.
         
        For a single core some part of the CPU is dedicated to TLB. In memory, our full page table exists where we're going to cache bits of translations to TLB. There might be multi-level TLBs and here we just imagine one TLB for simplicity. A TLB on the ordinary has tens or hundreds of entries.
        
     Translation: 
        VA: [  VPN  |   offset   ]
        CPU (more work to make thins run faster):
              - extract the VPN from the VA
              - lookup that VPN in TLB
              - if in there, we call it the "TLB hit"; meaning the translation is in the cache (much faster; its on the processor, small memory)
                  - then we get the PFN from the TLB and form the PA and do the memory reference
              - else (if not in there), "miss"; fetch the PTE from memory (slow)
                  - then its going to place it in the TLB (update TLB)
                  - retry instruction (memory reference); which it will get it in the TLB and then the fast case will uphold
                  
- Caches are based on "locality":
  - Spatial: if you access one thing likely to access things nearby
  - Temporal: if you access something now your more likely to access it in the future
- Data structures affect TLB behavior

          TLB Contents
            entries:
              [ PID | Valid bit | VPN-3 | PFN-100  ]
                              .
                              .
                              .
              [ PID | Valid bit | VPN-9 | PFN-200  ]
                           |
       Is this entry in the TLB in use? (little different from a PTE valid bit)  
       
       TLB is shared among different processes and how to make sure we get the right translations?
        - When we do a context switch from P1 -> P2: What to do?
            - Flush the TLB contents (set all entries to not valid). By making this P2 doesn't accidentally use a translation from P1
        - Could have additional bits with process IDs (PID) of the process the translation came from (can leave the contents in the TLB). Let's say for two processes that don't use very much entries in the TLB but are switching back and forth, they can co-reside in the TLB without an expensive flush in between running eachother.
              
       The hardware tends to work this way, all of these entries are searched in parallel (faster) that's what constraints the size of the TLB (if too big, hard to search quickly)
       
       
- Different way to structure the memory management support in hardware (in the 90s):
  - So far we have had a "hardware managed TLB" 
  - New thing: "OS/Software managed TLB"; the idea was that the hardware was complicated it has to know the structure of each page table then it has to build a handle on each instruction or memory reference and when it gets a "miss" it goes to the memory page table figure out where it is and then updates everything.
    - idea is that when its a **TLB hit**: still all hardware (common case(where we expect lots of hits) will be fast)
    - but on a **TLB miss** we're going to do something different: raise a **TLB miss exception**
      - Jumps to the OS exception handler
        - Consult the page table and find the right translation
        - Then updates the TLB (privileged instruction)
        - Return from the trap
      - Hardware now takes over: retries the instruction ("TLB hit")
    - **Freedom in OS**: the hardware doesn't know any details of the page table (can be of any structure we want cause the only entity accessing it now is the OS on a TLB miss); the hardware doesn't have to know where it lives (no PTBR), hardware is **simpler**.




