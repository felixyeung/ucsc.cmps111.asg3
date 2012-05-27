#include <stdlib.h>

#define PAGESIZE 4096      

int min (int* array) {
    int min = array[0];
    int i;
    for (i = 0; array[i] != 0; i++) {
        if (array[i] < min)
            min = array[i];
    }
    
    return min;
}

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
    
    int minPieceSize = min (parm2);
    int maxBitmapLen = slabSize / minPieceSize;
    
    // Calculate the number of bytes to allocate
    int bytesToAlloc = sizeof (struct space)
                       + sizeof (int) * sizeArrayLen
                       + sizeof (int) * numSlabs
                       + sizeof (char) * numSlabs * maxBitmapLen
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
                     + sizeof (char) * numSlabs * maxBitmapLen
    newSpace->end = newSpace->head + n_bytes - 1;
    newSpace->size = n_bytes;
    newSpace->minPageSize = PAGESIZE;
    newSpace->sizeArray = allocBlock
                          + sizeof (struct space);
    newSpace->numSlabs = numSlabs;
    newSpace->slabs = allocBlock
                      + sizeof (struct space)
                      + sizeof (int) * numSlabs;
    newSpace->bitmaps = allocBlock
                        + sizeof (struct space)
                        + sizeof (int) * numSlabs
                        + sizeof (int) * sizeArrayLen;
    newSpace->numBitmaps = numSlabs;
    
    // Fields we won't be using.
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
    
    // Zero out the bitmaps
    memset (newSpace->bitmaps, 0, numSlabs * maxBitmapLen);
    
    // Add newSpace to spaces.
    spaces[handle] = newSpace;
    
    return handle;
} 

void* sGetPointer (struct space* s, int objSize, int slabIdx, int slabOffset) {
    void* ptr = s->head;
    ptr += s->slabSize * slabIdx;
    ptr += objSize * slabOffset;
    return ptr;
}

void* smemalloc (int handle, long n_bytes) {
    struct space* s = spaces[handle];
    
    // Find the best fit from the array of sizes available.
    int fit = 0;
    int prevFit = 0;
    int i;
    
    for (;;) {
        // find the next best fit
        prevFit = fit;
        fit = 0;
        for (i = 0; s->sizeArray[i] != 0; i++) {
            if (s->sizeArray[i] >= n_bytes) {
                if ((fit == 0 || s->sizeArray[i] < fit)
                            && s->sizeArray[i] > prevFit)) {
                    fit = s->sizeArray[i];
                }
            }
        }
        if (fit == 0)
            // Then there is no size left unchecked.
            return NULL;
            
        // Iterate through size array, looking for an allocated slab of size ==
        // fit
        for (i = 0; i < numSlabs; i++) {
            if (s->slabs[i] == fit) {
                // Check to see if there are remaining spaces in the slab
                int j;
                numSlabParts = s->slabSize / s->slabs[i];
                for (j = 0; j < numSlabParts; j++) {
                    if (bitmaps[i][j] == 0) {
                        // We're good
                        break;
                    }
                }
                if (j < numSlabParts) {
                    // There is a free piece, so use it
                    bitmaps[i][j] = 1;
                    return sGetPointer (s, fit, i, j);
                }
            }
        }
        
        /* If we get here, there is no allocated slab of the size we desire.
         * After the best fit is checked, the results of this code will not
         * change with each iteration.
         */
        for (i = 0; i < numSlabs; i++) {
            if (s->slabs[i] == 0) {
                // Unallocated slab
                // TAKE IT AS YOUR OWN
                s->slabs[i] = fit;
                bitmaps[i][0] = 1;
                return sGetPointer (s, fit, i, 0);
            }
        }
    }
}

void smemfree (struct space* s, void* region) {
    void* offset = region - s->head;
    int slabIdx = (int) (offset / s->slabSize);
    int slabOffset = (int) ((offset % s->slabSize) / s->slabs[slabIdx]);

    /* Free the region */
    s->bitmaps[slabIdx][slabOffset] = 0;
    
    /* If all regions in the slab are free, free the slab */
    int i = 0;
    int numRegions = (int) (s->slabSize / s->slabs[slabIdx]);
    int isInUse = 0;
    for (i = 0; i < numRegions; i++) {
        if (s->bitmaps[slabIdx][i] != 0) {
            isInUse = 1;
        }
    }
    if (!isInUse) {
        s->slabs[slabIdx] = 0;
    }
}
