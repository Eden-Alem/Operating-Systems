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
    - Let's consider spin waiting(polling); where we wait until the status of the register changes, reading from the status register
      - Hardware mechanism; **interrupts**  

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




