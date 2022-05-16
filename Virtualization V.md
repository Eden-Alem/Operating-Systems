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

      Page Directory Entry (PDE)
      [Valid bit |     PFN      ]
            
    
    
    
