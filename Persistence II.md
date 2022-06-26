## Persistence
**RAID (Redundant Arrays of Independent Drives)**: As a slow operation to make the storage system faster, it allows multiple operations in parallel.

**File System**: is the piece of software in the OS that is responsible for managing a hard drive. 

- **RAID**: using many drives not one. Why?
  - Performance (bandwidth, IOPS(I/Os per second))
  - Reliability: by keeping many copies, tolerate disk failure
  - Capacity: more space
- **Interface**: RAID
  - looks like a disk. A hardware RAID (box with 16, 32 or 100s of hard drives inside them, CPU(controller), memory). 
  - its an array of blocks that we can read/write
- **Fault Model**: simple, how can we expect hard drives to fail?
  - drives have two modes: working, entirely not(broken); its easy to detect 
  - occassional real-time hard drive failures: bytes might get flipped, might byte shift (incorrectly written logic), lost writes (says it writes it but forgets to actually do it).
- RAID types: are called **levels**
  - Levels 0(striping, no redundancy),1(mirroring, n copies),2,3,4(parity-based),5(parity-based),6
  - **RAID-0**: Striping
  
        read/write=>array of blocks (let's consider blocks 0 - n-1)
        ___________________________
          [ 0 ] [ 1 ] [ 2 ] [ 3 ] -> striping these blocks accross drives
          [ 4 ] [ 5 ] [ 6 ] [ 7 ] => row is named offset
          [ . ] [ . ] [ . ] [ . ]
          HDD-0   1     2     3

        Mapping problem: interface address => disk,offset
        
        Calculation: its a simple static mapping
          disk: address % num_disks
          offset: address / num_disks
        
        Reliability: no redundancy, tolerate zero failures ["worse"]
        Capacity: "good", N disks each have D bytes = N*D
        Performance: types of access
          Sequential - read/write
            N disks: single disk gives S MB/s (seq I/O bandwidth) whether reading or writing cause its fairly symmetric to do either.
            Bandwidth we would get from reading/writing to all the disks: N*S MB/s -> parallel
          Random (small) read/write: N*R
            R MB/s (random I/O bandwidth); R < S
            
         Why do we stripe across the disks(horizontally) and not downwards(vertically)?
         - Parallelism (we get to utilize all the disks)
          
  - **RAID-1**: Mirroring
  
        [ 0 ] [ 0 ] [ 1 ] [ 1 ] 
        [ 2 ] [ 2 ] [ 3 ] [ 3 ] 
        [ . ] [ . ] [ . ] [ . ]
        HDD-0   1     2     3
        
        For each logical block we keep 2 (physical) copies.
        
        Capacity: N Drives, D bytes/drive = (N * D)/2; divide it by the number of copies we make
        Reliability: In our earlier fault model the whole drive could fail. We can essentially tolerate N/2 but we typically replace a failed hard drive after 1 failure (cause maybe if lets say disk 1 fails next time we could tolerate the failure of disk 2 or 3 since they're mirrored but if disk 0 fails we lose all of our data since our mirrored disk 1 had failed already)
        Performance: 
        From a latency perspective: writes to 2 drives is a little longer than writes to 1 drive. Reads => 1, from one disk.        
          Random I/O: bandwidth
            reads: N*R MB/s
            writes: (N/2)*R MB/s
              Single disk - R MB/s (~1MB/s)
          Sequential(large) I/O: bandwidth
            reads: (N/2)*S MB/s
            writes: (N/2)*S MB/s
              Single disk - S MB/s (~100MB/s)
            
  - **RAID-4**: Parity-based: XOR

        Parity disk: encode info about data disk; such that incase of a failure it tries to figure out what the data must have been on the data disk.
        Data Disks:                    Parity Disk:
        D0  D1  D2                          P
        1   0   0                           1
                              maintain an invariant for each row
                          (the idea is to make the # of 1's even in a row)
        1   0   1                           0
        1   1   1                           1
        0   0   0                           0
                            RAID-4 has just a single parity disk
          
        
        For simplicity we'll pretend the blocks are rather presented as its a bit but in reality a block could be 4KB and each would be done in a bit-level.
        
        Let's say if D2 had failed, how would we recover the data on D2?
        - We check if there are an even # of 1s (meaning if we have one 1 in a row it indicates the lost data must have been 1 since we had made sure that we had even # of 1s when constructing our parity). Similarly, computing the XOR would give us the resultant.

         D0    D1    D2    Parity
        [ 0 ] [ 1 ] [ 2 ]  [ P1 ] 
        [ 3 ] [ 4 ] [ 5 ]  [ P2 ] => blocks from 0 - n-1
        [ 6 ] [ 7 ] [ 8 ]  [ P3 ] 
        [ . ] [ . ] [ . ]  [ .. ]

        Reliability: can tolerate 1 disk failure
        Capacity: (N-1) * D MB
        Performance:
          Random:
          Single disk - R MB/s
            reads: (N-1)*R MB/s
            writes: R/2 MB/s -> no N times cause the system now runs in the rate of the parity disk and since we do 2 I/Os for every small write we don't get the full bandwidth out of the disk we get half.
            
            substractive calculation of parity:
            - read old data and read old parity
            - compare old data and new data, when it differs flip bit in old parity else do nothing
            - write new data and write new parity
            
            alternatively, the additive method is when you read data disks of the whole row not only the one you're updating and calculating the parity (but is less used as RAID rows grow, you have to read a lot of data).
            
            
        Latency: how long is RAID-4 write time? (assuming a single write/read: T)
          - read old data, old parity => I/O; happens in parallel
          - compute new parity (XOR; from old data, old parity, new data) => computation
          - write new data, new parity => I/O; in parallel
          => ~2T to do RAID-4 single write
          In RAID-4 when doing many small writes: Parity disk bottleneck; "small write problem" - we would like to read and write in parallel (when updating random blocks) but we have only one parity disk.
              
              
          Sequential:
          Single disk - S MB/s
            reads: (N-1)*S MB/s
            writes: quickly compute the parity and write them all in parallel through a row. (N-1) * S MB/s; its essentially the same as not having a parity disk just as striping.
              
        
  - **RAID-5**: Rotated Parity

        The idea is to make the parity rotated as follows:
        
         D0     D1     D2     D3
        [ 0 ]  [ 1 ]  [ 2 ]  [ P0 ] 
        [ 4 ]  [ 5 ] [ P1 ]   [ 3 ] => blocks from 0 - n-1
        [ 8 ] [ P2 ]  [ 6 ]   [ 7 ] 
        [ . ]  [ . ]  [ . ]   [ . ]
        
        Let's take an example, for instance in writing 8 and 2; we would consecutively use [8,P2] and [2,P0] which can be done in parallel utilizing all the disks solving for the "parity disk bottleneck(small write) problem"
        
        Capacity and Reliability: Same as RAID-4; we're using 1 parity block per row
        Performance:
          Single disk - R MB/s
          Random Reads: since we're using all the disks unlike RAID-4; 
                        N*R MB/s
          Random writes: we do 4 I/Os here; 2 reads and 2 writes
                         (N/4) * R MB/s
          
          Single disk - S MB/s
          Sequential reads: (N-1) * S MB/s; cause we skip a block when doing reading leaving off some part of the bandwidth same with writes.
          Sequential writes: (N-1) * S MB/s
        
        The cost of using RAID-5: we do 4 I/Os per logical write. So if small I/O write performance is an issue for the particular case(for example databases), mirroring is preferred as it is (N/2)*R MB/s.
        
        In relation to mirroring RAID-5 has better capacity, is good with large (sequential) I/Os but has a problem with small (random) writes in-terms of performance.
        
- **RAID**: summary
  - is a transparent way to build a faster, larger, more reliable disk
  - levels 0(no redundancy), 1(mirroring), 4 5 (parity-based), 2 (encoded array; not used), 3 (bit-level striping of each request), 6 (2 parity disks; used in practice - allows to reconstruct from a loss of a full disk and even one other block (incase of a corrupted block))
  - checksums: to detect and recover from corruption







