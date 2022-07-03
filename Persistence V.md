## Persistence
#### Log-structured File Systems (LFS)
- Hard drive performance led to LFS (major motivation)
  - Random (small) I/Os: Poorly (seeks, rotations => mechanical positioning)
  - Large sequential I/Os tend to perform well
  
- Problem (with our work loads):
  - Reads: hard to ensure large, contiguous. Deal with this by realizing that memory has gotten larger over time (main memory => 1990-1MB, now-manyGB). The importance of a larger memory lies on **caching** (main memory is cache for file data)
  - Writes: have control over it. So, when writing blocks of data from memory to disk and then if we got another request to update the blocks of data instead of writing them to a particular part of the disk (overwriting the blocks of data written earlier), we can write them to a new part of the disk. Then, we update some structure that tells us where the latest copy is. **Insight of LFS** => can write date anywhere.
  
  
