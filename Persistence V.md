## Persistence
#### Log-structured File Systems (LFS)
- Also known as "Copy on Write (COW)" (don't overwrite in place instead it creates a new copy on the disk) based file system.
- Hard drive performance led to LFS (major motivation)
  - Random (small) I/Os: Poorly (seeks, rotations => mechanical positioning)
  - Large sequential I/Os tend to perform well
  
- Problem (with our work loads):
  - Reads: hard to ensure large, contiguous. Deal with this by realizing that memory has gotten larger over time (main memory => 1990-1MB, now-manyGB). The importance of a larger memory lies on **caching** (main memory is cache for file data)
  - Writes: have control over it. So, when writing blocks of data from memory to disk and then if we got another request to update the blocks of data instead of writing them to a particular part of the disk (overwriting the blocks of data written earlier), we can write them to a new part of the disk. Then, we update some structure that tells us where the latest copy is. **Insight of LFS** => can write date anywhere.
  
  
        Basic Idea (focused on writes):
        1. Buffer many writes in memory (in-memory "segment" => a few MB in size)
        2. Write to the disk

        Example: Writing a file (with data blocks D1, D2, D3 and inode I)
        
            [[D1][D2][D3][I]]                   (memory)
        _____________|____________________________________
                [[D1][D2][D3][I]]                (disk)
                             |
           includes disk addresses of the data blocks
           
        Let's consider the case where a user overwrites D2 -> D2'
                    in-memory segment
                            |
                  [[D2'][I']-----]               (memory)
        ___________________________________________________
             [[D1][D2][D3][I]]     [[D2'][I']]    (disk)
                                          |
               newer version of the inodes which includes pointers to disk addresses of D2', D1, D3
               
        Problems:
          1. How to find the latest version of an inode?
            - We make use of a new in-memory structure named "inode map"
              - Could be an array, where for each inode number tells you the location of the latest version on the disk.
              Lets consider the example below with a file 'F' with inode #100 
        (4bytes for each inode(could be millions))   (the latest version)
                       not that big                  update this to 1001
                             |                                |
               F: inode map(imap) would represent as: 100 => 13 (for inode 100 of file F its located at 13)                      (memory)
               ______________________________________
                    [[D1][D2][D3][I]]   [[D2'][I']]    (disk)
               _______10__11__12__13_____1000_1001___
               
             When doing reads:
              - Directory contents: are the same as all the FS's we made use of before
                    inode(low-level) #
                           |
                [  "F"  | 100 ]
              To read the inode, we'll have one more additional step => we refer to the imap and look at the latest address(so as to go to the valid version of the inode). Reading is essentially the same (holding the fact that the imap resides in memory and that memory look-up is fast).

            Before in VSFS: to read an inode # of 100, we make use of the super block data (location of the inode table and size of each inode) and then we calculate the base + index (inode # (in this case 100) * size of inode), which is easy to locate where it is.
                
             => The other problem: imap resides in memory, where we're wary of crashes (where we could actually lose the entire inode map). Some options to deal with this:
                - Write out pieces of the inode map (makes sure its persistent)
                    When writing (let's say D1, D2, D3, I, we're going to write out a piece of the inode map => buffer in memory) along with the memory segment then we commit all these to the disk at once. But how do we find pieces of the inode map?
                        We're going to periodically write out one more data structure (called the checkpoint regoin kind of like our super block) where it has pointers to pieces of the inode map.
                        There are actually two checkpoint regions (CRs): the LFS alternates which one it updates with the latest pointers to pieces of the inode map. We make use of two, to avoid crashing when writing to one and corrupting the whole FS (dealing with crash consistency problem).
                  
                - Reboot: scan the entire disk and find the "lastest" of each inode and rebuild the inode map
           

          2. What to do with "garbage"(old versions of inodes, old versions of data)? 
             - "Background" process: means we have a thread within the OS that the FS created which is called the "cleaner" which reads old segments in and then determines what is live (actively used) and not. We can then write out the live data again and then free all the old segments.


          Let's consider our disk with the following segments:
          
             Before: [ S0 | S1 | S2 | S3 |          ]
            
             then, let's say the cleaner reads S0, S1 the subset of the 4 segments (which includes many blocks and inodes in them) to determine liveness. Then if it finds a little bit of liveness (like could be 1 of the segments), its going to write out S4 with that live info in it and then will free up S0 and S1.
             
             After: [    |    | S2 | S3 | S4 |         ]
             
             then the CRs will be updated to include info not only on the existing segments before but on segment 4 too.
             
             How to determine liveness of things within a segment?
               In a segment,
                [  | D3 | D2 | D1 | I |   ]
                          |
                how to tell this data block is live/dead?
                - Liveness for a data block, is if the data block is pointed to by an inode that's in the latest version of the inode map? (How to fast check this (what to record about a data block)? => knowing the offset alongside inode #). So, we'll record an extra info in each segment named the "segment summary block".

        Performance of LFS is determined:
          - by the sum of foreground and background I/O traffic
                  
        Fragmentation is another possible problem with LFS(as we overwrite the parts of a data blocks over and over agin we'll end up with parts of the data blocks in pieces all over the disk), how to handle it?   
        - With a fragmented file, we just read it and write it out in entirety (creates a new version of the file that is compacted).
        
### Solid-State Devices 
- are computer chips: serves as persistent (non-volatile) memory (can survive power loss)
- Flash-based SSDs: developed in the early 1980s in Japan.

##### Interface
- same as a hard-drive:
  - bunch of blocks (sectors)
  - we can read/write them
  
##### Internals
- SSD has many chips
- A chip is organized as follows:

        pages(2KB/4KB)
          |
        [ - - - |      |      |      ]     
        ________
            |
        block(sometimes named "erase" block)
        (256KB)
        
        
        When storing a bit in flash:
        - 'trapping charge' => inside the SSD, to differentiate whether the bit being stored is a 0 or 1.
        
        
        The operations to a flash: a little different than just read/write
        - read a page => gives data
        - update(2 steps):
              - erase (entire block): makes that block (pages inside of it) writeable (valid)
              - program a page (within block): set its contents. Once a page is programmed, to overwrite it we have to erase an entire block again.
              
         
        Performance characteristics:
        - reads: fast (10s of microseconds where in hard drives we're talking in milliseconds)
        - erase (block): slow (a few ms)
        - program (page):  somehow fast (100 microseconds)
        
        Reliability characteristics:
        - wear out(technology problem): erase/program a block too many times, it becomes unusable(adds maintainance cost).
        
        Design: SSD
        
                |
            interface; where we can read/write to it
                |
        [ [CPU]    [mem.]    ] => Flash Translation Layer(FTL)
        [ [F0] ------------  ]    - takes reads/writes to interface
        [ ---------- [Fn-1]  ]    - map them into low-level flash ops
                         |          (reads, erases, programs)
                   (flash chips)  
                   
         Bad way to build FTL: "direct mapped" - would map logical blocks to physical pages(Logical-0 <=> Page-0)
            - A read in this approach: just reads at the given address (easy)
            - Writes: lets consider writing address 2(updatig the contents of page 2):
                                    |
                [ P0 | P1 | P2 | P3 |        |         |          ] 
                 ------------------ |-------- --------- ----------
                      Block 0         Block1    Block2    Block3
                      
                - Read P0, P1, P3 (Put is somewhere but where?); could read them in memory (problem of crashing)
                - Erase Block-0
                - Program P0, P1, P2', P3
                
            - Its slow regarding performance (cause erases take ms)
            - Wear out (usage pattern controled) 

          So then what to do?
          - Design a better FTL: with logging (copy on write)
            - In SSDs, there is a memory where it includes a "map" that tells the last version for each block. So to do a logical write (4kB in size)-log style writing (start from left and filling it in going to right:
                  - Erase blocks once
                  - Program pages one at a time
                  - record in memory (in the FTL); logical block -> physical page mapping 

                                    |
                [ P0 | P1 | P2 | P3 |        |         |          ] 
                 ------------------ |-------- --------- ----------
                      Block 0         Block1    Block2    Block3   
                      
                Problems:
                  - Garbage
                  - How to recover the FTL translations after a crash
                  - Wear leveling (does log-structuring help with that? Somehow does, since it spreads writes across but if the data written to a block is never re-written to it again we wouldn't get the leveling of wearouts). Solution: Perioidcally read through the device, read and write it somewhere else and then erase that block making sure the erase load is even (in the FTL).
                  - How big is the FTL? (let's say with 1TB of SSD per page mappings with page: 1KB how many mappings do we have of logical blocks? 1TB/1KB ~ Billions of mappings with each let's say 4 bytes (too much memory space)). Solutions: better data structure, only keep "active(live)" pieces of the map in memory (caching, swapping), block mappings (refer to entire blocks not pages)  
                  
                  Why does locality matter in an SSD? 
                      - (on reads): number of mappings is reduced
                      
**Side note**: In-terms of performance doing large contiguous I/Os at once (have locality) performs better whether in SSDs or HDDs rather than random (small) I/Os
          
         
                 
    
    
    
    
