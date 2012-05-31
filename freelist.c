#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "space.h"

#define FF 0x00
#define NF 0x08
#define BF 0x10
#define WF 0x18
#define LIST_TYPE_FLAGS (FF | NF | BF | WF)

#define SIZE( _p ) (*((int*) (_p - 4)))
#define NEXT( _p ) (*((void**) (_p)))
// #define begin {
// #define end ;}

/* This function simply -4 from a list item*/
long getUsableSize(void* itemSpace) {
	return SIZE (itemSpace);
}

/*
 Return pointer to the start of actual freespace 
we waste 8 bytes, but who cares?
*/
void* getUsableLoc(void* freeSpace) {
	return NEXT (freeSpace);
}

void* getNextLoc(void* freeSpace) {
	return NEXT (freeSpace);
}

/*
void* getPervLoc(void* freeSpace)
	return freeSpace + 4;
}
*/

long getFreeSize(void* freeSpace) {
	long foo = SIZE (freeSpace);
	return foo;
}

void* allocIntoBlock(struct space* s, void* prev , void* target, long n_bytes) {	
	/* splitting */
	if (SIZE (target) > n_bytes + 4) {
		void* tmp =  target + n_bytes + 4;
		NEXT (tmp) = NEXT (target);
		
		SIZE (tmp) = SIZE (target) - n_bytes - 4; 
		/* prev now points to the newly splitted block */
    	if (prev != NULL)
    		NEXT (prev) = tmp;
    	else 
    		s->firstFree = tmp;
	}
	
	return target;
}

/*capture last free block to the left of region */
void* prevFree(struct space* s, void* region) {
	void* currFreeBlock = s->firstFree;
	void* prevFreeBlock = NULL;
	do {
		/* resolve the start of the block (used or un used) */
		if (region < currFreeBlock && region > prevFreeBlock) {
			return prevFreeBlock;
		}
		prevFreeBlock = currFreeBlock;
		currFreeBlock = NEXT (currFreeBlock);
	} while (currFreeBlock != NULL);
	return prevFreeBlock;
}

void* nextFree(struct space* s, void* region) {
	void* currFreeBlock = s->firstFree;
	void* prevFreeBlock = NULL;
	do {
		/* resolve the start of the block (used or un used) */
		if (region < currFreeBlock && region > prevFreeBlock) {
			return currFreeBlock;
		}
		prevFreeBlock = currFreeBlock;
		currFreeBlock = NEXT (currFreeBlock);
	} while (currFreeBlock != NULL);
	return NULL;
}

/* discover the left adjacent free block to region, if exists */
void* leftAdjacent(struct space* s, void* region) {
	void* currFreeBlock = s->firstFree;
	do {
		/* resolve the start of the block (used or un used) */
		if (currFreeBlock + getFreeSize(currFreeBlock) == region - 4) {
			return currFreeBlock;
		}
		currFreeBlock = NEXT (currFreeBlock);
	} while (currFreeBlock != NULL);
	return NULL;
}

void* rightAdjacent(struct space* s, void* region) {
	void* currFreeBlock = s->firstFree;
	do {
		/* resolve the start of the block (used or un used) */
		if (region + getFreeSize(region) == currFreeBlock - 4) {
			return currFreeBlock;
		}
		currFreeBlock = NEXT (currFreeBlock);
	} while (currFreeBlock != NULL);
	return NULL;
}

/*
 this is what a free list item looks like: 
[ freespace (4 bytes) | next (4 bytes) | data (n bytes) ]
*/
int fmeminit(int handle, long n_bytes, unsigned int flags, int parm1, int* parm2) {	
	//Our space model and actual free spaces are allocated here.
	size_t totalSpace = sizeof(struct space) + n_bytes;
	
	void* myBigBlock = malloc(totalSpace);
	if (myBigBlock <= 0)
		return -1;
	
	void* tmp_front = myBigBlock;
	
	
	/* point to space model and free space, we have no bitmaps for freelist!*/
	struct space* s = (struct space*) tmp_front;
	tmp_front += sizeof(struct space);
	
	/* start of free space */
    s->type = 'f';
	s->head = tmp_front;
	s->end = s->head + n_bytes - 1;
	s->size = n_bytes;
	
	/* during init, we only have one item in our list of free spaces */
	s->firstFree = s->head + 4;
	s->nextFree = s->firstFree;
	
	SIZE (s->firstFree) = n_bytes - 4;
	
	/* Init the listtype field */
	s->listType = flags & LIST_TYPE_FLAGS;
	
	/* 
	find the actual usable space, then place into it the next chuck of free ,space
	Note that since we have only one free Slot, this tracks the NEXT free space
	and it must point back to it self
	*/
	
	/*
	void* pervFreeSpace = getPervFree(s->firstFree); 
	pervFreeSpace = s->firstFree;
	*/
	
	NEXT (s->firstFree) = NULL;
	
	s->handle = handle;
	spaces[s->handle] = s;
	
	return s->handle;
}

void* fmemalloc(int handle, long n_bytes) {
	/* 512 is the max number of allocators we can have at once */
	struct space* s = spaces[handle];

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
	
	void* theBest = NULL;
	void* theBestPrev = NULL;
	long theBestSize = -1;
	
	void* theWorst = NULL;
	void* theWorstPrev = NULL;
	long theWorstSize = -1;
	
	void* region = NULL;
	switch (s->listType) {
		case FF:
			do {
				if (getFreeSize(currFree) >= n_bytes) {
					/* If we have a previous free block, we have to set its
					next pointer to the new free block*/
					region = allocIntoBlock(s, prev, currFree, n_bytes);
					break;
				}
				
				prev = currFree;
				currFree = getNextLoc(currFree);
			} while (currFree != NULL);
		break;
		case NF:
			do {
				if (getFreeSize(currFree) >= n_bytes) {
					region = allocIntoBlock(s, prev, currFree, n_bytes);
					break;
				}
				
				prev = currFree;
				currFree = getNextLoc(currFree);
				if (currFree == NULL)
					currFree = s->firstFree;
			} while (currFree != start);
		break;
		case BF:
			do {
				if ((getFreeSize (currFree) >= n_bytes)
                            && ((getFreeSize (currFree) < theBestSize)
                            || (theBestSize == -1))) {
					theBest = currFree;
					theBestPrev = prev;
					theBestSize = getFreeSize (currFree);
				}
				prev = currFree;
				currFree = getNextLoc(currFree);
			} while (currFree != NULL);
		break;
		case WF:
			do {
				if ((getFreeSize (currFree) >= n_bytes)
                            && ((getFreeSize (currFree) > theWorstSize)
                            || (theWorstSize == -1))) {
                    theWorst = currFree;
					theWorstPrev = prev;
					theWorstSize = getFreeSize (currFree);
				}
				prev = currFree;
				currFree = getNextLoc(currFree);
			} while (currFree != NULL);
		break;
	}   
    
	/* Actual allocation happens out here for best and worst fit since we need to read the entire array */
	if (s->listType == 0x10) {
        if (theBest == NULL)
            return NULL;
		region = allocIntoBlock(s, theBestPrev, theBest, n_bytes);
	}
	if (s->listType == 0x18) {
        if (theWorst == NULL)
            return NULL;
		region = allocIntoBlock(s, theWorstPrev, theWorst, n_bytes);
	}
	
	SIZE (region) = n_bytes;
	
	return region;
}

/*
We need to free the list and merge it with adjacent lists
*/
void fmemfree(int handle, void* region) {
	struct space* s = spaces[handle];

	int i;
	/* capture the space where region resides */
	
	/* clean some space*/
	//memset(region, 0, SIZE (region));
	
	void* left = prevFree(s, region);
	void* right = nextFree(s, region);
	
	/*link region after left */
	if (left != NULL)
		NEXT (left) = region;
	/*link region before right*/
	NEXT (region) = right;
	
	void* leftAdj = leftAdjacent(s, region);
	void* rightAdj = rightAdjacent(s, region);

	if (rightAdj != NULL) {
		SIZE (region) += SIZE (rightAdj) + 4;
		NEXT (region) = NEXT (rightAdj);
		if (s->nextFree == rightAdj) {
			s->nextFree = NEXT (rightAdj);
			if (s->nextFree == NULL)
			    s->nextFree = s->firstFree;
		}
	}
	
	/*increase the free space of left adjcent list */
	if (leftAdj != NULL) {
		SIZE (leftAdj) += SIZE (region) + 4;
		NEXT (leftAdj) = NEXT (region);
	}
	
	if (region < s->firstFree)
		s->firstFree = region;
}
