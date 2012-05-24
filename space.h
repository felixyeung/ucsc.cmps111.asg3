#ifndef SPACE_H
#define SPACE_H

struct space {
   char type; //’b’ forbuddy, ‘s’ for slab, ‘f’ for free list

   int handle; //handle number

   void *head; //pointer to start of mem space

   void *end; // pointer to end of mem space

   int size; //size of space

   void *bitmaps; /*pointer to an array of pointers to bitmaps,used in Buddy alloc*/

   int numBitmaps; /*how many bitmaps are we using? used in Buddy alloc*/

   void *first_free; //used in SLAB  and Free list

   int min_page_size; //min size of space
};

#endif
