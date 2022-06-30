## Persistence
### File Systems: Implementation
- **Crash**: What do we mean by crash?
  - Power loss (UPS, uninterruptible power supply)
  - Kernel panic/bug
  - user restarts
- Why is it important for FS?
  - Might be in the middle of an update
  
        Examples: Updates
        
        => File creation: What data structures are updated?
          - inode bitmap (to find a free inode and mark it used)
          - inode (initialize all its various contents)
          - update root directory data (directory: mapping of a name => low-level name; update the data block to have the new file's mapping)
          - root inode: update times
          [if the directory had to grow to a new block, we could also]
            - data bitmap; new data block
            
        => Append a new data block to an existing file:
          => How to do this at an API level? (system calls)
          => Which blocks are written to the disk as a result?
          
               inode   data             
               bitmap  bitmap              data blocks
          [    |      |      |         |                 ]
           super                inode
           block                table
           
          - The API side:
            Let's consider opening an existing file in a write only mode. For every process there's a file descriptor table which points to a struct file (open file table entry; referring to an in-memory inode)
            int fd = open ("/lo", O_WRONLY); 
                      |
                  lseek(fd, 0, SEEK_END); -> position where we're reading/writing in the file arbitrarly
                      |              4KB
                  write(fd, buffer, size);
                      |
                  close(fd);   
          - Data/metadata, we need to write:
              
              data bitmap     data block    inode table  |
              [         ]     [        ]    [         ]  | in memory
            _______________________________________________
               data      inode            data           |
              bitmap     table           blocks          | in disk
                []   |     []    |     |\\|            ] |     
                
          From the perspective of having a data, inode and data bitmap; how many different orders could we have in writing them to disk?
            Orders of writing: 
            _______________________________ 
                D    D   I   I   DB   DB_____(what happens if it crashes 
                I    DB  D   DB  I    D______here,what data is on the disk,
                DB   I   DB  D   D    I      can it recover?)
            _______________________________
            
            Crashes could occur before or after writing everything as the above lines indicate (which we won't focus on)
            
            Let's consider crash scenarios:
                _______________________________ 
                    D    D   I   I   DB   DB 
               _____1____3___5__________________
                    I    DB  D   DB  I    D
               _____2____4______________________
                    DB   I   DB  D   D    I      
                _______________________________

                crash-1,3: Wrote the data block then sys. crashed, how does the disk look to it?
                    - Looks good to the OS: the FS metadata is consistent
                Crash-2: inode and data bitmap don't agree: inconsistent
                crash-4: inconsistent: no inode that points to the data blocks (so when deleting a file when we move to the inode to free up the data blocks there's nothing pointing to the blocks hence "space leak")
                crash-5: inconsistent: could read garbage (or zeroes, privacy problem: info leak)
                
          Solutions:
            - Check: is the FS consistent? Usually done before usage (mounting; making it accessible) of the FS. Named the File System Check (FSCK); used for many years but disks got large (RAID too) and to check them took hours (too slow).
            - Journaling: (based from ideas in databases called write-ahead logging). Does the eager approach (does some work with every update and recovery after crash, its fast)
            
         1. write note to log/journal about pending update
         2. write the update                               (in memory)
        ______________________________________________________________    
          [        |     |     |     |         |              ]
           Journal                                          (in disk)
        (write-ahead
            log)
             |
         added another on disk structure that the FS will manage; its persistent (we make sure to write the note on the log on disk).
         
         Faster recovery; much smaller space to look over and we know what to do (looking at the pending updates)
         
         Example to better illustrate the details of journaling (appending to an existing file):
         
         (info about the update; like locations)
         Transaction
         begin         [DB]      [I]       [D]              (in memory)
         _|_____________________________________________________________
         [\|DB I D|\|   |   |     | [DB] |    [I]    |     |D|          ]   
          Journal  |                                         (on disk)
              Transaction
                  end
                  
         The problem we faced with the FS when it comes to crashes is that we wanted that atomic behavior of updating (writing everything to disk) at once but having some of it to update and causing those inconsistencies was undesirable (The mindset we have with FS is ALL OR NOTHING - Atomicity). Journaling makes sure of this; it pushes all the updates to the log and makes sure we get them all done or not. We have steps for journaling:
         1, We write all updates out to the journal PERSISTENTLY on disk.
         2, We have the updates (writes) in place (to the DB, I, D)
         
         CRASHES:
            - During step 2 (after step 1): we replay the transactions in the journal (recover from log).
            - During step 1 (issue all writes to journal): the recovery process might copy (replay) garbage later on to the FS (corrupt super block; not be able to mount).
              - To solve the above problem (linux ext3; older version of the journaling FS) was took step 1 and broke it into two parts:
                  a) Write the transaction begin block and the contents but not the end block to the journal then wait until these are complete then
                  b) Write the transaction end block (transaction committed at this point) then we wait till its complete
                  Then, we go ahead and do step2 (in place updates)
                  
                The above two parts (a and b) are slow cause we wait in the middle till operations are done and so in 2005 a group of researchers suggested that we make use of a checksum on the transaction begin and end block (calculation over the contents of the updates and allow the FS to see if the checksum matches when it comes back to recovery; was known as a transaction checksum which got implemented by linux ext4); was possible to issue all those writes and we could know its validity through the checksum.

         Side Note: What the disk guarantees is the writing of a sector atomically but larger writes may be partially complete.
         
         
         
         
             
           
            



