
====================================
Storage Manager Implementation 
====================================

Design Concept 
-----------------------------------------------------------

- Implement a basic page file management system supporting data transfer between disk files and memory  
- Use binary files to store fixed-size (4096 bytes) pages  
- Use file handles (SM_FileHandle) to manage file information: file name, total page , current page position, file pointer  
- Page handles (SM_PageHandle) point to page data in memory  
- Adopt uniform return codes (RC) for error handling, with parameter validity checks before operations 


Code Structure 
-------------------------------------------------------------
1. File Operation Functions  
   - initStorageManager: Initialize the storage manager  
   - createPageFile: Create a new file containing an empty page  
   - openPageFile: Open a file and set up the file handle  
   - closePageFile: Close the file  
   - destroyPageFile: Delete the file  

2. Page Reading Functions  
   - readBlock: Read the specified page  
   - getBlockPos: Get the current page position  
   - readFirstBlock/readPreviousBlock/readCurrentBlock/readNextBlock/readLastBlock: Read pages at specific positions  

3. Page Writing Functions  
   - writeBlock: Write to the specified page  
   - writeCurrentBlock: Write to the current page  
   - appendEmptyBlock: Append an empty page  
   - ensureCapacity: Ensure the file contains the specified number of pages
-------------------------------------------------------------------------
 Name：Weiming Lin 
 Cwid ： A20478106 

