#ifndef SPACE_H
#define SPACE_H

#define NUM_SPACES 512 

struct space {
    char type; //'b' for buddy, 's' for slab, 'f' for free list

    int handle; //handle number

    void *head; //pointer to start of mem space

    void *end; // pointer to the last byte of mem space

    int size; //size of space

    int minPageSize; //min size of space, used for buddy and slab
    
    /*** buddy allocator ***/
    int numBitmaps; // how many bitmaps are we using?
    
    char**** bitmaps; // pointer to an array of bitmaps.

    /*** freelist allocator ***/
    int listType; // Type of freelist allocator

    void *firstFree; // First free space in the freelist

    void *nextFree; //used in next fit
    
    /*** slab allocator ***/
    int* sizeArray; /* Array of object sizes to be allocated (for slab). Like
                     * parm2, it is NULL-terminated.
                     */

    int numSlabs; // Size of the slab array

    int* slabs; // Array corresponding to the set of slabs

    int slabSize; // Size of a slab
};

extern struct space** spaces;

#endif
