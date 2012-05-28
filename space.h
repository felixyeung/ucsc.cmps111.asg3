#ifndef SPACE_H
#define SPACE_H

#define NUM_SPACES 512

struct space {
   char type; //'b' for buddy, 's' for slab, 'f' for free list

   int handle; //handle number

   void *head; //pointer to start of mem space

   void *end; // pointer to the last byte of mem space

   int size; //size of space

   void *bitmaps; /* pointer to an array of pointers to bitmaps, used in
                   * Buddy and slab alloc
                   */

   int numBitmaps; /* how many bitmaps are we using? used in Buddy and
                    * slab alloc
                    */

   int listType; //Used for freelist

   void *firstFree; //used in SLAB  and Free list

   void *nextFree; //used in next fit

   int minPageSize; //min size of space

   int* sizeArray; // Array of object sizes to be allocated (for slab)

   int numSlabs; // Size of the slab array

   int* slabs; // Array corresponding to the set of slabs

   int slabSize; // Size of a slab

   void *first_free; //used in SLAB  and Free list

   int min_page_size; //min size of space
};

#endif
