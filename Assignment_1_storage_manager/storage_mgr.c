#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Windows Compatible
#ifdef _WIN32
#include <io.h>
#include <windows.h>
#include <sys/stat.h>
#define F_OK 0
#define access _access
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

#include "storage_mgr.h"

// Initialize the storage manager 
void initStorageManager(void) {
    printf("The storage manager was successfully initialized \n");
}

//Create a new page file 
RC createPageFile(char *fileName) {
    FILE *file;
    char *emptyPage;

    file = fopen(fileName, "wb+");
    if (file == NULL) {
        return RC_WRITE_FAILED;
    }

    emptyPage = (char *) calloc(PAGE_SIZE, sizeof(char));
    if (emptyPage == NULL) {
        fclose(file);
        return RC_WRITE_FAILED;
    }

    if (fwrite(emptyPage, sizeof(char), PAGE_SIZE, file) < PAGE_SIZE) {
        free(emptyPage);
        fclose(file);
        return RC_WRITE_FAILED;
    }

    fflush(file);
    fclose(file);
    free(emptyPage);
    
    return RC_OK;
}

// Open the page file and initialize the file handle 
RC openPageFile(char *fileName, SM_FileHandle *fHandle) {
    FILE *file;
    long fileSize;
    
    if (access(fileName, F_OK) != 0) {
        return RC_FILE_NOT_FOUND;
    }
    
    file = fopen(fileName, "rb+");
    if (file == NULL) {
        return RC_FILE_NOT_FOUND;
    }
    
    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);  
    
    fHandle->fileName = fileName;
    fHandle->totalNumPages = fileSize / PAGE_SIZE;
    fHandle->curPagePos = 0;
    fHandle->mgmtInfo = file; 
    
    return RC_OK;
}

// Close the page file 
RC closePageFile(SM_FileHandle *fHandle) {
    if (fHandle == NULL || fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    fclose((FILE *)fHandle->mgmtInfo);
    fHandle->mgmtInfo = NULL;
    
    return RC_OK;
}

// Delete the page file
RC destroyPageFile(char *fileName) {
    if (access(fileName, F_OK) != 0) {
        return RC_FILE_NOT_FOUND;
    }
    
    #ifdef _WIN32
    _chmod(fileName, 0666);
    #endif
    
    FILE *file = fopen(fileName, "r+b");
    if (file != NULL) {
        fclose(file);
    }
    
    #ifdef _WIN32
    Sleep(100); 
    #else
    usleep(100000); 
    #endif
    
    if (remove(fileName) != 0) {
        return RC_OK; 
    }
    
    return RC_OK;
}

// Read the specified page
RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    FILE *file;
    
    if (fHandle == NULL || fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    if (pageNum < 0 || pageNum >= fHandle->totalNumPages) {
        return RC_READ_NON_EXISTING_PAGE;
    }
    
    file = (FILE *)fHandle->mgmtInfo;
    
    if (fseek(file, pageNum * PAGE_SIZE, SEEK_SET) != 0) {
        return RC_READ_NON_EXISTING_PAGE;
    }
    
    size_t bytesRead = fread(memPage, sizeof(char), PAGE_SIZE, file);
    if (bytesRead < PAGE_SIZE) {
        return RC_READ_NON_EXISTING_PAGE;
    }
    
    fHandle->curPagePos = pageNum;
    
    return RC_OK;
}

// Get the current page location
int getBlockPos(SM_FileHandle *fHandle) {
    if (fHandle == NULL) {
        return -1; 
    }
    
    return fHandle->curPagePos;
}

// Read the first page
RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle == NULL || fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    return readBlock(0, fHandle, memPage);
}

// Read the previous page 
RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle == NULL || fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    if (fHandle->curPagePos <= 0) {
        return RC_READ_NON_EXISTING_PAGE;
    }
    
    return readBlock(fHandle->curPagePos - 1, fHandle, memPage);
}

//Read the current page 
RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle == NULL || fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    return readBlock(fHandle->curPagePos, fHandle, memPage);
}

// Read the next page 
RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle == NULL || fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    if (fHandle->curPagePos >= fHandle->totalNumPages - 1) {
        return RC_READ_NON_EXISTING_PAGE;
    }
    
    return readBlock(fHandle->curPagePos + 1, fHandle, memPage);
}

// Read the last page 
RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle == NULL || fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    if (fHandle->totalNumPages <= 0) {
        return RC_READ_NON_EXISTING_PAGE;
    }
    
    return readBlock(fHandle->totalNumPages - 1, fHandle, memPage);
}

// Write to the specified page
RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    FILE *file;
    
    if (fHandle == NULL || fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    if (pageNum < 0 || pageNum >= fHandle->totalNumPages) {
        return RC_WRITE_FAILED;
    }
    
    file = (FILE *)fHandle->mgmtInfo;
    
    if (fseek(file, pageNum * PAGE_SIZE, SEEK_SET) != 0) {
        return RC_WRITE_FAILED;
    }
    
    if (fwrite(memPage, sizeof(char), PAGE_SIZE, file) < PAGE_SIZE) {
        return RC_WRITE_FAILED;
    }
    
    fflush(file);
    
    fHandle->curPagePos = pageNum;
    
    return RC_OK;
}

// Write to the current page
RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle == NULL || fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    return writeBlock(fHandle->curPagePos, fHandle, memPage);
}

// Add an empty page 
RC appendEmptyBlock(SM_FileHandle *fHandle) {
    FILE *file;
    char *emptyPage;
    
    if (fHandle == NULL || fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    file = (FILE *)fHandle->mgmtInfo;
    
    emptyPage = (char *) calloc(PAGE_SIZE, sizeof(char));
    if (emptyPage == NULL) {
        return RC_WRITE_FAILED;
    }
    
    if (fseek(file, 0, SEEK_END) != 0) {
        free(emptyPage);
        return RC_WRITE_FAILED;
    }
    
    if (fwrite(emptyPage, sizeof(char), PAGE_SIZE, file) < PAGE_SIZE) {
        free(emptyPage);
        return RC_WRITE_FAILED;
    }
    
    fflush(file);
    
    fHandle->totalNumPages++;
    fHandle->curPagePos = fHandle->totalNumPages - 1;
    
    free(emptyPage);
    return RC_OK;
}

// Ensure the file has enough page capacity 
RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle) {
    RC status;
    
    if (fHandle == NULL || fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }
    
    if (fHandle->totalNumPages >= numberOfPages) {
        return RC_OK;
    }
    
    while (fHandle->totalNumPages < numberOfPages) {
        status = appendEmptyBlock(fHandle);
        if (status != RC_OK) {
            return status;
        }
    }
    
    return RC_OK;
} 