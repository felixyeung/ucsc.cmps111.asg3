#include <stdlib.h>
#include <math.h>
#include "space.h"

/* This function simply -4 from a list item*/
PRIVATE long getUsableSize(long itemSpace) {
	return itemSpace - 4;
}

/*
 Return pointer to the start of actual freespace 
we waste 8 bytes, but who cares?
*/
PRIVATE void* getUsableLoc(void* freeSpace)
	return *freeSpace;
}

PRIVATE void* getNextLoc(void* freeSpace)
	return *freeSpace;
}

/*
PRIVATE void* getPervLoc(void* freeSpace)
	return freeSpace + 4;
}
*/

PRIVATE long write(void* freeSpace)
	long foo = *(freeSpace - 4);
	return foo;
}

PRIVATE long getFreeSize(void* freeSpace)
	long foo = *(freeSpace - 4);
	return foo;
}

PRIVATE void* allocateIntoBlock(struct space* s, void* prev , void* target, long n_bytes) {
	if (prev != NULL)
		*prev = target + n_bytes + 4;
	else 
		s->firstFree = target + n_bytes + 4;
	
	*(target + n_bytes) = *(target - 4) - n_bytes - 4; 
	return target;
}
/*
 this is what a free list item looks like: 
[ freespace (4 bytes) | previous (4 bytes) | nextfree (4 bytes) | ... ]
*/
PRIVATE int fmeminit(long n_bytes, unsigned int flags, int parm1, int* parm2) {
	int i;
	//Our space model and actual free spaces are allocated here.
	size_t totalSpace = sizeof(struct space) + n_bytes;
	void* myBigBlock = malloc(totalSpace);
	void* tmp_front = myBigBlock;
	
	
	/* point to space model and free space, we have no bitmaps for freelist!*/
	struct space* s = tmp_front;
	tmp_front += sizeof(struct space);
	
	/* start of free space */
	s->head = tmp_front;
	s->end = s->head + n_bytes - 1;
	s->size = n_bytes;
	
	/* now, put out metadata of how much freespace into */ 
	*s->head = getUsableSpace(n_bytes);
	
	/* during init, we only have one item in our list of free spaces */
	s->firstFree = s->head + 4;
	s->nextFree = s->firstFree;
	
	/* 
	find the actual usable space, then place into it the next chuck of free ,space
	Note that since we have only one free Slot, this tracks the NEXT free space
	and it must point back to it self
	*/
	
	/*
	void* pervFreeSpace = getPervFree(s->firstFree); 
	pervFreeSpace = s->firstFree;
	*/
	
	*s->firstFree = NULL;
	
	/* TODO: factor out handler assignnment to the meminit switch function */
	/* assign a handler to our space struct*/
	
	int handler;
	/* 512 is the max number of allocators we can have at once */
	for (i = 0; i < 512; i++) {
		if (spaces[i] != NULL) {
			handler = i;
			spaces[i] = s;
			break;
		}
	}
	
	s->handler = handler;
	
	return s->handler;
}

PUBLIC void* fmemalloc(int handler, long n_bytes) {
	/* First, find the handler */
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

	/* iterate over the list until we see that the next free slot points back to front */
	void* currFree;
	void* start;
	void* prev = NULL;
	if (s->listType != 0x08) {
		currFree = s->firstFree;
		start = s->firstFree;
	}
	else {
		currFree = s->nextFree;
		start = s->nextFree;
	}
	/*
		0x0 first fit
		0x08 next fit
		0x10 best fit
		0x18 worst fit
	*/
	
	void* theBest;
	void* theBestPrev;
	long theBestSize;
	
	void* theWorst;
	void* theWorstPrev;
	long theWorstSize;
	
	switch (s->listType) {
		case 0x0:
			do {
				if (getFreeSize(currFree) >= n_bytes) {
					/* If we have a previous free block, we have to set its
					next pointer to the new free block*/
					allocIntoListItem(s, prev, currFree, n_bytes);
				}
				
				prev = currFree;
				currFree = getNextLoc(currFree);
			} while (currFree != start);
		break;
		case 0x08:
			do {
				if (getFreeSize(currFree) >= n_bytes) {
					allocIntoListItem(s, prev, currFree, n_bytes);
				}
				
				prev = currFree;
				currFree = getNextLoc(currFree);
				if (currFree == NULL)
					currFree = s->firstFree;
			} while (currFree != start);
		break;
		case 0x10:
			do {
				if (getFreeSize(currFree) >= n_bytes && getUsableSize(currFree) < theBestSize) {
					theBest = currFree;
					theBestPrev = prev;
					theBestSize = getUsableSize(currFree);
				}
				prev = currFree;
				currFree = getNextLoc(currFree);
			} while (currFree != start);
		break;
		case 0x18;
			do {
				if (getFreeSize(currFree) >= n_bytes && getUsableSize(currFree) > theWorstSize) {
					theWorst = currFree;
					theWorstPrev= prev;
					theWorstSize = getUsableSize(currFree);
				}
				prev = currFree;
				currFree = getNextLoc(currFree);
			} while (currFree != start);
		break;
	}
	
	/* Actual allocation happens out here for best and worst fit since we need to read the entire array */
	if (s->listType == 0x08) {
		allocIntoListItem(s, theBestPrev, theBest, n_bytes);
	}
	if (s->listType == 0x18) {
		allocIntoListItem(s, theWorstPrev, theWorst, n_bytes);
	}
}

PUBlIC void fmemfree(void* region) {
	int i;
	/* capture the space where region resides */
	for (i = 0; i < 512; i++) {
		if (region > spaces[i]->head && region < spaces[i]->end) {
			struct space* s = spaces[i];
			switch (s->listType){
				case 0x0;
				break;
				case 0x08;
				break;
				case 0x10;
				break;
				case 0x18;
				break;
			}
		}
	}
}