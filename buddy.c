#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "space.h"

int isPowerOfTwo (unsigned int x) {
 while (((x % 2) == 0) && x > 1) /* While x is even and > 1 */
   x = x >> 1;
 return (x == 1);
}

int bmeminit(int handle, long n_bytes, unsigned int flags, int parm1, int* parm2) {
	int i;
	
	/* This is the maximum number of pages we can get from n bytes, where parm1 is the minimum size of a page */
	int numPages = 1 << (parm1);
	
	/* if numPages is not a power of two, then there's no way to continue. */
	if (!isPowerOfTwo(numPages))
		return -1;
	
	/* calculating number of  Bitmaps we need */
	int numBitmaps = 0;
	int tmp;
	for(tmp = numPages; tmp > 1; tmp = tmp >> 1) {
		numBitmaps += 1;
	}
	
	/* Since we can only call malloc once, this is the total space we need */
	int initMapLength = 2;
	int mapsSize = 0;
	for (i = 0; i < numBitmaps; i++) {
		mapsSize += sizeof(int) * initMapLength;
		initMapLength *= 2;
	}
	
	size_t totalSpace = sizeof(struct space) + sizeof(void*) * numBitmaps + mapsSize;
	
	/* This is the space everything goes into */
	void* myBigBlock = malloc(totalSpace);
	void* tmp_front = myBigBlock;
	
	/* our space struct resides in the front of myBigBlock */
	struct space* s = tmp_front;
	tmp_front += sizeof(struct space);
	
	/* create now point to the space after the space struct and put out bitmaps in it */
	s->bitmaps = tmp_front;
	tmp_front = s->bitmaps + sizeof(void*) * numBitmaps;
	
	initMapLength = 2;
	
	/* Now loop over and create maps of size 2 and greater*/
	for (i = 0; i < numBitmaps; i++) {
		s->bitmaps[i] = tmp_front;
		
		/*clear bitmap*/
		memset(s->bitmaps[i], 0, sizeof(int) * initMapLength);
		
		tmp_front += sizeof(int) * initMapLength;
		initMapLength *= 2;
	}
	
	/* THIS IS WHERE ACTUAL DATA GOES */
    s->type = 'b';
    
	s->head = tmp_front;
	
	s->end = s->head + n_bytes - 1;
	
	s->size = n_bytes;
	
	s->minPageSize = parm1;
	
	/* we need to know how many bitmaps we have */
	s->numBitmaps = numBitmaps;
	
	s->firstFree = NULL;
	
	/* assign a handler to our space struct*/
	
	s->handle = handle;
	spaces[s->handle] = s;
	
	return s->handle;
}
