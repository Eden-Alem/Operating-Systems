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
  - while (status == busy); //spin
  - write data => data register
  - wrtie commad => command register //device begins operation for this command
  - while (status == busy) //spin (wait)



