## Persistence
### File Systems: Implementation
- Simple: Very Simple File System (based on earliest UNIX file systems)
- Locality: Fast File System
- Crash Consistency: has to do with the persistence of the disk (we need to recover the data structures on disk after rebooting when it comes to file systems; unlike memory which clears up everytime the system starts-up the memory management doesn't care about the data structures in memory). 
  - Journaling (Write Ahead Logging) (JFS); ext3/4 (common linux file system)
- Write performance: in some cases its more important than reads (workloads that only do writes like backups; caching in systems makes reads effective unlike writes)
  - Logging: Log-Structured File System (LFS)
- Flash-based solid state drives (SSDs)

#### Abstraction
- **Files**: an array of bytes; each file has a low-level name (number)
  - Operations: open, read/write, unlink, close 
- **Directories**: a way of organizing files and other directories; list of <names, low-level names> (list of mappings of human-readable names to low-level names)
- Directory Hierarchy (FS Tree): starts at the root directory(/)

#### Implementation
- On-disk data structures
- Access Methods: open, read/write, etc...


###### Very Simple File System
- FS: has an API (routine) and sends out read/write to disk blocks

      How are disk blocks structured?
             block
               |
      Disk - [   |   | ------- |   ] -> divided into n-SSDs or 512bytes sectors; bunch of blocks
      
      What needs to be on the disk?
      - Data
      
      In VSFS, we know where data blocks live:
      
                      X--"data blocks"->
      [   |   | ----- |   |   | --------- |   ]
        0   1          100 101             max
        
      Block size: must be a multiple of sector size (512 bytes) 
      Block is the minimal unit of allocation to a file/directory
      
      Tradeoffs for large and small block sizes:
      -> Large block size: in-terms of performance disks are efficient with sequential data (large amount). It might be efficient to organize data into large sectors but its easy to end up with internal fragmentation (waste)
      -> Small block size: better in-terms of less internal waste but many blocks will be used for a large file
      
      So, its better to use blocks of size => 4KB; matches page size (good property)
      super 
      block 
        | allocation
        | _____|__ _info__ _______data blocks_____
      [   |   |   | ----- |   |   | --------- |   ]
        0   1   2     |    100 101             max
        |         (array of
       4KB         inodes)
        
      1. Data
      2. Info about each file: per file will have a data structure on the disk (called inode -  index node); inode array (inode table)
        What's inside of inode?
        - type: regular file or directory
        - ownership
        - permissions: read/write/execute
        - timestamps
        - size, # of blocks
        - pointer to a particular disk data block (location of blocks of this file/directory)
        array of inodes:
        0 ......... N-1 -> index value
        |
       low-level name of a file => inode number (index value)
       
      Given an inode number: what do we need to calculate the exact location of a particular inode?
      - size
      - base (where the array starts)

      3. directories: are treated just like files
            - each has an inode and may point to one or more data blocks
            - data blocks have specific contents: name (human readable), low-level name(inode number)
            - each directory has entries ..(parent), .(this) etc.
            
      4. In-order to do allocation: we need to track what's free or allocated (we need two blocks here); bitmaps
         - 1st block: inode bitmap
         - 2nd block: data bitmap
         Bitmap: is a block of 32K bits marked 0 or 1 based on whether the corresponding entity is free or in use.
         
      5. The first block on the system (0): "Super block"; with per file system info    
         - block size, type(like ext4), inode table starting point (location), data blocks starting point(location) etc....
         - Super block is cached in the memory of the system; thus don't need to reffer to the disk for accessing these info all the time.
      

##### Access Methods
The file system API and what it does to interact with the disk?

    "creat" system call (eg; creating the file "/lo.txt")

    Assume at the beginning: an empty file system
    
    super 
    block 
      | allocation
      | _____|__ _info__ _______data blocks_____
    [   |   |   | ----- |   |   | --------- |   ]
      0   1   2     |    100 101             max
          |   |  (array of
      inode   data   inodes)
         bitmaps  
         
     Are there files in an empty file system? The (empty) root directory
          - in the data block: lets consider 0 to be the inode # of root
                  [  ..  | 0 ] -> no parent so the same low-level #
                  [   .  | 0 ]
                  [lo.txt| 1 ] => Final outcome for creating a file
                  [          ]
                  
     Now to create /lo.txt in the root directory:
     1. File system will first check the permissions of the user to do so (read the inode of the root directory). To locate the inode of the root directory we check the super block (in this case 0) then do a "read" to the disk on the block containing inode-0
     2. Check that the filename of the new file is unique ("read(s)" to the data blocks of the root directory) 
     3. Look through for space for inode:
          - "read" the inode bitmap
     4. Mark a bit (to show that its in use) [in memory]
     5. "write" to inode bitmap
     6. Update the inode of lo.txt ("read" the block of the inode array then update that in memory then "write" that out to the block of the inode that contains lo.txt)
     7. Link low-level inode number to a human readable name ("read" root data then update it to [lo.txt->1] - add a directory entry then "write" our directory block out to disk)
     => Creation: DONE
     
     To update the inode of the new file: we read a block of data, update the inode then we write out the inode block; why do we do read -> modify -> write as opposed to just write (in a byte addressable memory system)?
     - When doing something smaller than the unit access of the system (like in updating a bit; you have to take out (read) the whole byte, update the bit and then write the whole byte back on to memory(byte addressable system)). And to update an inode (usually 128bytes); we read a chunk of inodes update the one and then write them all out.
     
    When writing to a file:
        - if its off the end (appending data to it): we have to go through an allocation (growing the file in size)
        - if we're overriding a block of a file: don't have to go through allocation, we know how to write to it.
        
    Opening a full(absolute) path name ("/d1/d2/d3/lo.txt"): the more separators (the longer) the more work to open this file: we check permissions alongside; looking through all levels to open the file, the below process, is called "path traversal"
    - read root inode
    - read root dir data  ->  [  ..  | 0 ] 
                              [   .  | 0 ]
                              [  d1  |10 ]  
    - read d1's inode
    - read d1's data block(s) ->  [      |   ] 
                                  [  d2  |12 ]  
    - read d2's inode
    - read d2's data block(s) 
    - read d3's inode
    - read d3's data block(s) ->  [      |   ] 
                                  [lo.txt|71 ] 
    - read lo.txt's inode
    
    => Once file is open: 
          - per-process structure: file descriptor (integer) will refer to an open file table (that indicates the permissions of the file (readable, writable, executable), where the current offset is) which then refers to an in-memory version of inode that is then used to consult where the data blocks of the file are on the file system (in a UNIX based file system)
          
          
     What about large files?
     - inode structure: 128bytes
       [ type   ]
       [ size   ]
       [ blocks ]
       [ perm.  ]
       [ owner  ]
       [ time   ]
       [        ] |
       [ 100    ] |-> pointers(address) (to disk blocks)
       [ 101    ] |
       [ 102    ] |
       [  .     ] |
       [        ]-indirect pointer (used when the above blocks fill up)
           |___________ will point to a block of full of pointers
       [        ]       [  1000  ]|
           |            [  1001  ]|- 4KB         => 1024 more blocks
       double indirect  [    .   ]| 
         pointer        [    .   ]|  
             |          ----------
             |            4bytes - each pointer
             |
    double indirect block   indirect block  data block
          [       ] -----> [       ] -----> [       ]   
          [       ].       [       ].       [       ]   
          [       ]\       [       ] --->[       ] data    
          [       ].\ indirect block     [       ] block  
                     \ [       ].    data block
                       [       ]---> [       ]    
                       [       ].    [       ]   
                       
                       
      Mapping: File -> Block
        - read/write [current offset (byte offset)]
        File system translates: byte offset => block(s)
        Given a byte address in a linear order (of each pointers as the representation above) we can determine which block we're reffering to then do the writes/reads (byte address is converted to a low-level block number looking through the tree - like a multi-level page table :D)
    
     Most files are small; thus results in an imbalanced tree structure of the file system (direct pointers to the data blocks may be enough for small-sized files then for larger files we then allocate indirect pointers for additional direct pointers to data blocks)


#### Caching and Write Buffering
- To make file systems fast we use **memory**
- It's important on reads: 
  - Opening a long nested file requires many reads of directory blocks, but if we cache inodes, directory data(meta data) in memory; meaning opening the file again (re-opening) is fast
  - For large files, in inodes it would have direct pointers (that point to data blocks) and additional indirect blocks. And so in doing reads we might have to read double indirect blocks, indirect blocks and data; and so caching the "double indirect blocks, indirect blocks" will make it faster (even for newer data since we've already read the indirect blocks). Cache data too (for subsequent reading of the same data).
- Writing:
  - open() -> write() .... write() -> close()
  - At this point, the FS has done no I/O to the disk instead its buffering data in memory. Then later in the background, the OS finally writes out this data to the disk. Why is buffering these writes useful? 
    - Its faster
    - Subsequent reads might make use of it
    - Aggregate many small writes to block-sized writes (minimize system call overheads; sometimes known as "batching effect")
    - Over-write the same block repeatedly (reduces the amount of I/O; wrote the final version)
    - Disk Scheduling (performs better with more writes)
    - If file is deleted, we don't have to write to disk at all.
  - Why is write buffering not good?
    - Crash means data loss
  - If persistence (immediate writes) matter for a particular system:
    - You call fsync(): open()->write()....write()->fsync() => which forces data to disk(slow but safe)
- File System Cache is shared across all processes (reading and writing the same data by different processes is reffering to the same data block)
- In MACs, fsync actually doesn't write it to the disk immediately it writes it to the drive DRAM which is allocated alongside the disk then writes it on the persistent storage(disk) later on.

 


    
    


