## Virtual Memory
- Page Tables: array-based called "linear page table"
  - Problems:
    - Paging(even with a lot of hardware support): was too slow (extra memory reference to the page table itself)
      - Solution: TLB (Translation lookaside buffer) "address translation cache"
        -  high performance hardware located on the CPU where you give it a VPN it'll search through all entries and give back a translation *translation hit* and if not we have to consult memory and access the page table and move forward
    - Page tables are too big
    - What if the amount of memory accessed by a program is greater than the amount of physical memory?
      - Use a disk of some kind (slower larger storage medium)

**Problem #2:** Page tables are too big (linear)  

     32-bit virtual address space (4GB of addressable space):
         VPN                 Offset
    [    22bits         |    10bits     ] 
    
    Page size: 1KB; 2^10
    
    How big is the page table?
    array: one entry per VPN
    pages: 2^22 ~ 4million
    we have about 4 million entries
    
    PTE / Page Table Entry    
        can we read, write 
            or execute
                |
    [ valid | protection | PFN ]
       |                    |
    (goal: not waste    most of the
    physical memory)       PTE
    
    Sometimes we share code pages (like shells and editors; many instances) all are sharing the same code(same exact binary). So we have one physical representation of that binary and we'll have all our virtual page tables point to that same one physical instance (in that case we might want to mark code as readonly, read/execute only); the heap is going to be private for every process so that it can be shared
    
    PTE: 4bytes
    
    Total size of page table: 4million * 4bytes ~ 16MB
    
    One page table per process: assume 100 processes, 1600MB of page tables
    
              VAS  [8 pages]
          0  [//////////] - code 
          1  [----------] - heap
          2  [          ] |
          3  [          ] |
          4  [          ] |- unused
          5  [          ] |
          6  [          ] |
          7  [----------] - stack
    
    Our page table for the above process (mirrors the structure of our address space):
            
          valid bit 
            [ 1  |   10   ]
            [ 1  |   11   ]      
            [ 0  |        ]|
            [ 0  |        ]|
            [ 0  |        ]|- expansive way of representing redundant info (invalid pages)
            [ 0  |        ]|
            [ 0  |        ]|
            [ 1  |   23   ]
            
**Solutions:**

- Multi-level page table
- Hybrid: Segmentation and Paging (read more about it)
- Inverted page tables: single page table which has one entry for every physical frame (like a hashtable); one per system(not process)
 
All of the above proposed solutions are just *data structures*; for building a more space efficient data structure

**Multi-level page table:** Tree-like data structure

- We want to figure out for some chunks in between on how not to allocate pages for them
- We're going to add one level more of a tree and name it the **page directory**
- Our page directory will have an entry for each page of the page table; will have an entry named page directory entry(PDE)

      The page table below is divided into pages and each page is composed of a bunch of PTEs 
      
      Our page directory is then going to have an entry (a mapping) of each page of the page table
      
      The first and last page of the page table has PTEs some of which are valid, and the middle pages are all non valid (which we want to remove for our space efficient representation on the physical memory)
      
      The non-valid pages need not be represented in physical memory
      
      
         Page Table (Linear)
           Valid 
            bit
          [  1             ] - PTE |             (Multi-level Version)
          [  1             ] 0     |-Page      One level more of a tree:
          [  0             ]       |               Page Directory    
       ------------------------ | 
          [  0             ] 1  |-Page         [ 1 |/////////////] 0
          [  0             ]    |              [ 0 |             ] 1
       ------------------------ |              [ 0 |             ] 2
          [  0             ] 2                 [ 0 |             ] 3
          [  0             ]                   [ 0 |             ] 4
       ------------------------                [ 0 |             ] 5
          [  0             ] 3                 [ 0 |             ] 6
          [  0             ]                   [ 1 |/////////////] 7
       ------------------------                  |
          [  0             ] 4             Page Directory  
          [  0             ]                 Entry(PDE)
          [  0             ]  
       ------------------------             Each PDE constitutes
          [  0             ] 5          [ Valid bit |       PFN       ]
          [  0             ]                  |              |        
       ------------------------      notifies if there is    |  
          [  0             ] 6     a valid PTE on this page  |
          [  0             ]          of the page table      |
       ------------------------                              |
          [  0             ] 7                   where the page of the
          [  0             ]                   page table resides on the
          [  1             ]                       physical memory


      Multi-level page table assumes that you generally have a very sparse address space which is not always the case; the downside of this solution

Multi-level page table even makes physical memory management easier for the OS than linear page tables? Why is that the case? (Side note to consider: Is this a faster lookup of our translation or a slower? It's slower)

- Multi-level page table is slower but smaller (tradeoff: space vs time) but cause we use hardware (TLB) the translation is not that bad. 
- We have to allocate contiguous space for the whole page table while using linear page table and inorder to do that it should be available to us. How do we find translation with a linear page table? We have the PTBR(page table base register) identifies where it begins (its hard to find a contiguous big chunk for allocating these page tables).
- In multi-level even the page table has been divided into pages (so when we need to allocate an address space we do so for the page directory and then as pages get allocated for code and stack we allocate a few more pages for those pieces of the page table). 
- Its easier to find space when its divided to pages than it is to find space for one large contiguous array.


**Problem #3:** Virtual memory accessed by programs is greater than physical memory of the system

- Memory Hierarchy:
  - CPU(constitutes of these forms of memory like registers, caches); fast; expensive; small
  - Main memory
  - Solid-state drive (SSD; smaller and faster than a hard drive)
  - Hard drive; slow; cheap; large
- Hardware: Figures out which data to cache on the CPU (for faster access and high performance) and which data to store on main memory so that it is accessed less frequently. The compiler is also involved in what to store on the registers within the CPU
- OS: when too many pages are being accessed and memory gets full the OS needs to decide to move some pages to a slower storage medium

**How the OS makes for the illusion of a very large sparse address space?**

- **Mechanisms:**
  - swap space (on disk; larger slower storage which could be SSD or Hard drive) 

        Swap space:
         0 1            n-1
        [ | |   |//|    | ]
                 |       |
                 |     blocks -> page sized (often 4KB)
                 |_______  
        Memory: (m < n)  |
                _________ (swap/page this in memory before usage) 
         0 1   |    m-1
        [ | | |\\|  | ]
               |
         Initially, another info already exists on the memory so before "page in" we need to first "swap/page out" this already existing data to the disk storage then page in the data we need to memory from the disk.
         
         Then we update our page table to indicate which PFN the date now resides in.
         
        
        Virtual address space
        0   [   ] -> valid (in memory)
            [   ] -> invalid
              .
              .
            [///] -> valid (not in memory, swapped out to disk)
        max [   ] 
        
        How to track a valid page not in physical memory?
        - add some information to the page table

        PTE
        [ v |   protection   |     P    | PFN ]
          |          |             |
        valid  is it readable/   Present 
                 writable/         bit
                executable? 
                
        if v=1, p=1 -> valid, PFN is the location of the page
        if v=1, p=0 -> valid, page is on disk somewhere
            we often call that "page miss/page fault"
            
        When p=0, the "swap space address" is placed in the PFN
   
**Side note:** In disks we refer to blocks and in memory pages           
         
         
**Abstraction of a virtual memory** 
    
    Every memory reference will go through this process
    
    TLB(translation lookaside buffer) resides within the CPU
    Hardware: Takes the VA(Virtual Address) and changes it to as below
            [ VPN |    Offset    ]
      Then it consults the TLB:
      - Best case (happens usually): results in a TLB hit
            Retrieve the PFN directly and append the offset to it
                [ PFN | Offset ]
            Then access memory (load, store, instruction fetch)  
      - TLB miss:
            Fetch PTE (page table entry)
            Check the Validity? if not: Seg-fault
            Check protection? if not: protection-fault
            Is it present(in memory)? if not: page-fault
                At this point the OS page fault handler runs
                  Locate a free physical frame
                    if not: "page out" existing page
                    -> this process is named "page replacement"
                  Page/Swap in the desired page from disk
                  Update the PTE: (P=1, PFN=new allocated address)
                  Retry instruction
    
 
- **Policy:**
  - Page replacement:
    - When the OS needs to remove a page, which one to do so?
      - Least Recently Used (LRU)    | better in understanding,
      - Least Frequently Used (LFU)  | hard to implement(track & update)
      - Most Recently used (MRU): if searching through a lot of data, restrict the pages used and makes use of MRU
      - Random   | easy to
      - FIFO     | implement
    - Comparison: Optimal Policy
      - Kick out the page that will be used furthest in the future 

**Example:** Memory size - 3 pages

Then we access; 1  1  2  0  1  3  0  1  2  0   (h-hit, m-miss) hit or miss? What's in memory?

- FIFO        : m  h  m  m  h  m  h  m  m  m - doesn't do well(few hits)
  - In memory : 1    21 021   302  130 213 021
- LRU(in mem.):          1  2  0  1  3  0  1
-    (in mem.):       1  2  0  1  3  0  1  2
- MRU(in mem.): 1  1  2  0  1  3  0  1  2  0
  - hit/miss  : m  h  m  m  h  m  h  h  m  h - do better (more hits)


##### How to implement "approximate" LRU?
*Why approximate?* LRU requires knowledge of access time of each page. Timestamp per page, then the hardware updates it on each memory access, then replacement is done: "LRU"

For replacement, search through the timestamps of each page and find the least recently used page (which is really "slow")

So people approximate LRU: try to kick out not necessarily the least recently used page but rather a pages that is "not recently used"

Hardware: When a page is accessed sets a single bit (reference bit) indicating that the page has been accessed recently

OS: When doing replacement - Scans through the pages and if the reference bit == 0, means "Page/Swap out" this page (found the page we're replacing); and if the reference bit == 1, set the reference bit to 0 and continue scanning


**Problem of swapping (paging in/out):** "Disk" is too slow; memory - nanoseconds while hard drive - milliseconds (million times slower). Swapping is a less utility than it used to be due to its slow behavior than in certain desktops nowadays and most systems prefer more memory than allowing swapping.

So the usual solution is buy more memory (don't swap): but does more memory always lead to a better cache "hit rate"? ;)

So lets consider a sequence of page memory references as: 1 2 3 4 1 2 5 1 2 3 4 5 and in running a FIFO with size 3 vs. a FIFO with size 4

We won't percieve better hits when the size is larger :O this is called **Belady's Anomaly**; discovered years ago when people were studying on memory access patterns. But still for some policies bigger sizes is of no problem (considering LRU its hit rate performance is better or the same for bigger caches; this is cause its ordering it from LRU-MRU in its cache resulting in it being atleast as good as the cache with a smaller size. Thus depends on the policy.)


**Side note of the day:** Computer systems are not perfect. Bits get changed periodically ,alpha particles fly through systems and actually flip bits. The way to deal with this considering "reliable computing" is to use redundancy (storage and computation across different machines) afterall don't underestimate probabilistic occurences :D



