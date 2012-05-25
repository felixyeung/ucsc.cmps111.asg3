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

    int slabSize = parm1 * PAGESIZE;
    int numSlabs = n_bytes / slabSize;
    int bytesToAlloc = sizeof (struct space) + sizeof (int) * numSlabs
                + n_bytes;
    void* allocBlock = malloc (bytesToAlloc);
    
    // Initialize the struct.
    struct space* newSpace = allocBlock;
    newSpace->type = 's';
    newSpace->handle = handle;
    newSpace->head = allocBlock + sizeof (struct space)
                + sizeof (int) * numSlabs;
    newSpace->end = newSpace->head + n_bytes - 1;
    newSpace->size = n_bytes;
    newSpace->minPageSize = PAGESIZE;
    newSpace->numSlabs = numSlabs;
    newSpace->slabs = allocBlock + sizeof (struct space);
    
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
    
    // Add newSpace to spaces.
    spaces[handle] = newSpace;
    
    return handle;
}
    
