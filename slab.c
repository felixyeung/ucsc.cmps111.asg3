#include <stdlib.h>

#define PAGESIZE 4096        

int smeminit (long n_bytes, unsigned int flags, int parm1, int* parm2) {
    // Find an open spot in spaces.
    int i;
    for (i = 0; i < 512; i++) {
        if (spaces[i] == NULL) {
            break;
        }
    }
    
    int handle = i;
    if (handle == 512) {
        // No spot in spaces was empty, so return error.
        return -1;
    }

    // Get the size of parm2, to be used when making sizeArray
    int sizeArrayLen = 1;
    while (parm2[sizeArrayLen - 1] != 0) {
        sizeArrayLen += 1;
    }
    
    // Calculate the number of slabs
    int slabSize = parm1 * PAGESIZE;
    int numSlabs = n_bytes / slabSize;
    
    // Calculate the number of bytes to allocate
    int bytesToAlloc = sizeof (struct space)
                       + sizeof (int) * sizeArrayLen
                       + sizeof (int) * numSlabs
                       + n_bytes;
    void* allocBlock = malloc (bytesToAlloc);
    
    // Initialize the struct.
    struct space* newSpace = allocBlock;
    newSpace->type = 's';
    newSpace->handle = handle;
    newSpace->head = allocBlock
                     + sizeof (struct space)
                     + sizeof (int) * numSlabs
                     + sizeof (int) * sizeArrayLen;
    newSpace->end = newSpace->head + n_bytes - 1;
    newSpace->size = n_bytes;
    newSpace->minPageSize = PAGESIZE;
    newSpace->sizeArray = allocBlock
                          + sizeof (struct space);
    newSpace->numSlabs = numSlabs;
    newSpace->slabs = allocBlock
                      + sizeof (struct space)
                      + sizeof (int) * numSlabs;
    
    // Fields we won't be using.
    newSpace->bitmaps = NULL;
    newSpace->numBitmaps = 0;
    newSpace->listType = 0;
    newSpace->firstFree = NULL;
    newSpace->nextFree = NULL;
    
    // Each slab should be unallocated, so mark it as such.
    for (i = 0; i < newSpace->numSlabs; i++) {
        newSpace->slabs[i] = 0;
    }
    
    // Copy parm2 into sizeArray, including the NULL-termination
    for (i = 0; i < sizeArrayLen; i++) {
        newSpace->sizeArray[i] = parm2[i];
    }
    
    // Add newSpace to spaces.
    spaces[handle] = newSpace;
    
    return handle;
}

void* smemalloc (int handle, long n_bytes) {
    
}
