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
          [  1             ] - PTE |         (Multi-level Version)
          [  1             ] 0     |-Page   One level more of a tree:
          [  0             ]       |            Page Directory    
       ------------------------ | 
          [  0             ] 1  |-Page      [ 1 |/////////////] 0
          [  0             ]    |           [ 0 |             ] 1
       ------------------------ |           [ 0 |             ] 2
          [  0             ] 2              [ 0 |             ] 3
          [  0             ]                [ 0 |             ] 4
       ------------------------             [ 0 |             ] 5
          [  0             ] 3              [ 0 |             ] 6
          [  0             ]                [ 1 |/////////////] 7
       ------------------------               |
          [  0             ] 4           Page Directory  
          [  0             ]               Entry(PDE)
          [  0             ]  
       ------------------------           Each PDE constitutes
          [  0             ] 5        [ Valid bit |       PFN       ]
          [  0             ]                |              |            
       ------------------------    notifies if there is    |  
          [  0             ] 6   a valid PTE on this page  |
          [  0             ]        of the page table      |
       ------------------------                            |
          [  0             ] 7                  where the page of the
          [  0             ]                 page table resides on the
          [  1             ]                      physical memory


      Multi-level page table assumes that you generally have a very sparse address space which is not always the case; the downside of this solution

Multi-level page table even makes physical memory management easier for the OS than linear page tables? Why is that the case? (Side note to consider: Is this a faster lookup of our translation or a slower? It's slower)

- Multi-level page table is slower but smaller (tradeoff: space vs time) but cause we use hardware (TLB) the translation is not that bad. 
- We have to allocate contiguous space for the whole page table while using linear page table and inorder to do that it should be available to us. How do we find translation with a linear page table? We have the PTBR(page table base register) identifies where it begins (its hard to find a contiguous big chunk for allocating these page tables).
- In multi-level even the page table has been divided into pages (so when we need to allocate an address space we do so for the page directory and then as pages get allocated for code and stack we allocate a few more pages for those pieces of the page table). 
- Its easier to find space when its divided to pages than it is to find space for one large contiguous array.






 
