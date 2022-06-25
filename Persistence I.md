## Persistence
- More generally related with input/output, disks, SSDs and software in OS (file system)
- We still make use of Hard Drives (Hard Disk Drives, HDDs) cause of cost; its been around since 1950s.

#### Devices
- many types: hard drive, SSD, other storage types (NAND, NOR), keyboard/mouse, sound, graphics, network

      [ CPU ]           [ Memory ]      
         |                   |
      ---------------------------- Memory bus
                |
      ---------------------------- I/O (PCI) bus
         |                 |
         |              Network
      ---------------------------- SCSI (Small Computer Systems Interconnect) or SATA
                |        |
               HDD .... HDD 
               
      The above hierarchy is needed for building cost-effective systems.
               
      Example: intel z270 chipset
          
##### Device Interface 

    Device controller (hardware that presents interface and implements device functions)
    [                           ]
    [ [data] [command] [status] ] -> registers
    [                           ]
    [        internals          ]
    [ (microcontroller, memory) ]
    
    
- **Protocol** for accessing a device
  - while (status == busy); //spin => from status register
  - write data => data register
  - wrtie commad => command register //device begins operation for this command
  - while (status == busy) //spin (wait)

- **Inefficiencies** on the above protocol
  - spin waiting (wasting CPU; for eg a hard drive takes around 10ms for a request)
  - data movement (to send certain megabytes of write data request, the CPU takes time transferring the data itself; it copies it from the memory and transfers it to other connected devices like the network interface as an example)
- How to do I/O?
  - How to read/write registers on device?  
    - Let's consider spin waiting(polling); where we wait until the status of the register changes, reading from the status register. In which we make use of the hardware mechanism; **interrupts**.  

                P1, process: blocked on I/O
            CPU ------ read() [     |      ] (unblocks process) -----------
                         |     could run P2      |
                     File system                 |
                         |                       |
               Device driver (I/O low level)     | interrupt
                         | issue I/O command     | handler
            Disk         -------------------------
              - The reason we need interrupts is for better CPU utilization as the diagram above illustrates and to better deal with devices that have variable latencies.

            CPU ------ write()
                         |
                        FS -> I/O device driver
                         |
                   [ copying data ]  ------------------ 
                  (from memory through the CPU        |
               down to the device; slow operation)    |
                               |                     Disk            
            reduce the CPU burden with more specialized hardware
                         (Direct Memory Access)

            Direct Memory Access, DMA - is a copying engine that can move data from devices to memory or vice versa; where the CPU tells the device the buffer that needs to be moved and which direction it is and the specialized computing engine(DMA) does the data movement. During this transfer, considering virtual memory management the OS needs to make sure that the pages inside memory don't get re-organized (make sure it stays there; pinned page, can't be swapped out or given to another process)
            
- How to interact with device?
  - I/O instructions (specific inst. provided by the hardware to allow access to registers) 
  - Memory mapped I/O (no special instructions, loads and stores can be used; as the hardware maps the device registers on to memory and then reading and writing in to these memory spaces will be changed to reads and writes into the devices by the hardware)


##### Hard Drives
A hard drive is a persistent storage device (nonvolatile in contrast with DRAM)

- From "CS" perspective:
  - Controller: control operation
  - Mechanics: disk platters, arm, etc... 

- **Interface** to a hard drive: is similar to memory
  - read: will specify sector, length -> return data
  - write: will specify sector, length, data -> return an error code
  - Modern storage devices have an abstract interface where they are represented as a series of sectors (each 512 bytes) and each sector has an address (0-n)
- **Internals:** a hard drive has 
  - one or more platters (hard metal could be made from alumnium) which is magnetically coded on both sides so that we can encode bits(1s, 0s) into sector sized units. Separated with certain spaces each round platter is connected to a spindle at the center (the spindle cuts through each round platter through their center connecting all) and at the bottom of those platter(s) is a motor connected to the bottom tip of the spindle. The motor runs at a fixed rate (typical: 7200(10K) RPM(rotations per minute)). Data is organized into a bunch of concentric circles (where each circle is named a **track**) on the platter(s) so that we can read/write the data. And each track is divided into sectors.
  - The controller needs a mapping of the interface => platter, surface, track.
  - On the side of these platters, there is another motor attached to it where rod that attaches the motor and the platter(s) is named the **disk arm**. In which it swings back and forth (to move between tracks, to land on the desired data) and at the tip of it on the platter side its attached to the read/write head (where in which it either changes the bits on the magnetic surface as they rotate under or senses the bits and read them as the bits rotate under the head). Scanning back and forth through the arm is called seeking. 

        Side view of the platter(s)
        
               read/write head
              arm   |
        []     |    |     | -> spindle
        []---------[/]    |
        []    ------------|------------(side view of each round platter)
        []---------[/]    |
        []                |
        []---------[/]    |
        []    ------------|------------
        []---------[/]----|-> aerodynamic env't: inducing a magnetic change
        []                |         as the bits fly by
        []---------[/]    |
        []    ------------|------------          
        []---------[/]    |
         |              [///] -> motor; which spins it a fixed rate
        motor 
        (that swings the arms back and forth)
        
        - When we have multiple platters as the above diagram shows, we will have multiple arms extended on both the surface of each platter but generally only one will be active for reading/writing data at a time.

- In the process of read/write it essentially has 3 components:
  - **Seek**: positioning of the read/write head over the correct track. Tracks are packed very densely; weird fact: you can fit hundreds of tracks on the width of the human hair :D. How does it know where to seek to? The hard drive will encode information on the surface that tells it the track and sector info as a whole positioning info so it constantly reads the right info to land at the right place.
  - **Rotational delay**: waiting for the desired sector to rotate under the head
  - **Transfer**: do read/write. We have the over head of seeking and rotational delay unlike memory for transferring data (a well designed system will spend most of its time transferring and will cause less over head to exist)

        Seek            Rotate            Transfer
        
        Ti/o = Tseek + Trotate + Ttransfer
        
        large I/Os vs. small I/Os
        transfer: 100MB/s
        seek avg: 7ms
        rot. avg: 3ms
        1MB I/Os vs 1KB I/Os; what's the performance of these? 
        
        For large I/Os, for 1MB:
        1MB(1sec/100MB)(1000ms/1sec) = 10ms -> transferring 1MB
        seeking and rotating: 7ms + 3ms = 10ms
        50% of our peek ability
            -> 1MB/20ms -> (1MB/20ms)(1000ms/1sec) = 50MB/s
        
        For 1KB of I/O (small I/Os) the over head time still holds 10ms, but the time taken to transfer is approximately 0.
        1KB/10ms(1000ms/sec) = 100KB/s = 0.1MB/s
        
        







