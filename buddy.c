#include <stdlib.h>
#include <math.h>

PRIVATE int isPowerOfTwo (unsigned int x) {
 while (((x % 2) == 0) && x > 1) /* While x is even and > 1 */
   x /= 2;
 return (x == 1);
}

PRIVATE int handlerIsInUse(int handler) {
	int i;
	/* 512 is the max number of allocators we can have at once */
	for (i = 0 ; i < 512; i++) {
		if (handler != i)
			return 0;
	}
	return 1;
}

PUBLIC int bmeminit(long n_bytes, unsigned int flags, int parm1, int* parm2) {
		int i;
		
		/* This is the maximum number of pages we can get from n bytes, where parm1 is the minimum size of a page */
		numPages = n_bytes / parm1;
		
		/* if numPages is not a power of two, then there's no way to continue. */
		if (!isPowerOfTwo(numPages))
			return -1;
		
		/* calculating number of  Bitmaps we need */
		int numBitmaps = 0;
		int tmp;
		for(tmp = numPages; tmp > 0; tmp >> 1) {
			numBitmaps += 1;
		}
		
		/* Since we can only call malloc once, this is the total space we need */
		int initMapLength = 1;
		int mapsSize = 0;
		for (i = 0; i < numBitmaps; i++) {
			mapsSize += sizeof(char) * initMapLength;
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
		
		initMapLength = 1;
		
		/* Now loop over and create maps of size 2 and greater*/
		for (i = 0; i < numBitmaps; i++) {
			s->bitmaps[i] = tmp_front;
			tmp_front += sizeof(char) * initMapLength;
			initMapLength *= 2;
		}
		
		/* THIS IS WHERE ACTUAL DATA GOES */
		s->head = tmp_front;
		
		//s->end = s->head + n_bytes;
		
		s->size = n_bytes;
		
		s->minPageSize = parm1;
		
		/* we need to know how many bitmaps we have */
		s->numBitMaps = numBitmaps;
		
		s->firstFree = NULL;
		
		/* assign a handler to our space struct*/
		
		int handler;
		/* 512 is the max number of allocators we can have at once */
		for (i = 0; i < 512; i++) {
			if (!handlerIsInUse(i)) {
				handler = i;
				break;
			}
		}
		
		s->handler = handler;
		
		return s->handler;
}

PUBLIC void* bmemalloc(int handler, long n_bytes) {
	int i;
	/* 512 is the max number of allocators we can have at once */
	struct space* s;
	
	for (i = 0; i < 512; i++) {
		if (spaces[i]->handler == handler) {
			s =  spaces[i];
			break;
		}
		return NULL;
	}
	 
	int currentBlockSize = s->minPageSize;
	for (i = s->numBitMaps - 1; i >= 0; i--) {
		/* Find out if n bytes would fit into our current block lvl */
		if (n_bytes <= currentBlockSize)
			break;
		else
			/* prepare to iterate over the next level, with double the block size */
			currentBlockSize *= 2;
	}
	
	/* local map */
	int* map = s->bitmaps[i];
	int lengthOfMap = 2 ^ i;
	
	/* becareful, we are reusing i */
	for (i
}