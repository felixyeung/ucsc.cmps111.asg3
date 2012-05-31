#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "space.h"

int isPowerOfTwo (unsigned int x) {
 while (((x % 2) == 0) && x > 1) /* While x is even and > 1 */
   x /= 2;
 return (x == 1);
}

<<<<<<< HEAD
int PowerOf (unsigned int x, unsigned int y) {
int i;
int z=1;
for(i=0;i<=y;i++){
   z = z*x;
   }
 return (z);
}

=======
>>>>>>> 10affc5621ee9e9fbccbb07f1f861c6bc54f54e8
void markDown(long n_bytes, char** maps, int mapIdx, int blockIdx, int blockSize) {
	if (n_bytes <= blockSize) {
		maps[mapIdx][blockIdx] = 1;
		markDown(n_bytes, maps, mapIdx + 1, blockIdx * 2, blockSize / 2);
	}
}

void* allocAndMark(struct space* s, long n_bytes, char** maps, int mapIdx, int blockIdx, int blockSize) {
	int mapSize = 1 << (mapIdx + 1);
	int j;
<<<<<<< HEAD
	printf("map %d\n",mapIdx);
	for (j = blockIdx; j < mapSize; j++) {
		/* we try to allocate at the current level, figure out which buddy we are*/
		int buddy;
=======
	for (j = blockIdx; j < mapSize; j++) {
		/* we try to allocate at the current level, figure out which buddy we are*/
		int buddy;
		
>>>>>>> 10affc5621ee9e9fbccbb07f1f861c6bc54f54e8
		if (j % 2 == 0)
			buddy = j + 1;
		else
			buddy = j - 1;
			
		/* we have an in-use buddy, we can allocate */
<<<<<<< HEAD
		if (maps[mapIdx][j] == 0) {
			maps[mapIdx][j] = 1;
=======
		if (maps[mapIdx][buddy] == 1) {
			maps[mapIdx][j] = 1;
			
>>>>>>> 10affc5621ee9e9fbccbb07f1f861c6bc54f54e8
			/* TODO: resolve the block size somehow */
			void* slot = s->head + (blockSize * j);
			
			/* WE need to travel back down 
			*/
			markDown(n_bytes, maps, mapIdx + 1, j * 2, blockSize / 2);
			
			return slot;
<<<<<<< HEAD
		}
	}

=======
			
		}
		else 
			return allocAndMark(s, n_bytes, maps, mapIdx - 1, j / 2, blockSize * 2);
	}
>>>>>>> 10affc5621ee9e9fbccbb07f1f861c6bc54f54e8
	return NULL;
}

void freeAndMergeBuddies(char** maps, int mapIdx, int b1) {	
	/*first free the first buddy */
	
	maps[mapIdx][b1] = 0;
	
	/* then, let's look at our buddy */
	int b2;
	if (b1 % 2 == 0) {
		b2 = b1 + 1;
	}
	else {
		b2 = b1 - 1;
	}
	
	/* This function will resolve the parent of b1 b2, then recurr*/
	if (maps[mapIdx][b2] == 0) {
		int parent = b1 / 2;
		freeAndMergeBuddies(maps, mapIdx - 1, parent);
	}
}

int bmeminit(int handle, long n_bytes, unsigned int flags, int parm1, int* parm2) {
	int i;
<<<<<<< HEAD
	int p;
	p=PowerOf(2,parm1);
	printf("p %d\n",p);
	/* This is the maximum number of pages we can get from n bytes, where parm1 is the minimum size of a page */
	int numPages =(n_bytes/p);
	// printf("pages %d\n",numPages);
	// printf("n_bytes %d\n",n_bytes);
	// printf("pages %d\n",parm1);
=======
	
	/* This is the maximum number of pages we can get from n bytes, where parm1 is the minimum size of a page */
	int numPages = n_bytes / parm1;
	
>>>>>>> 10affc5621ee9e9fbccbb07f1f861c6bc54f54e8
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
	
<<<<<<< HEAD
	printf("bit maps%d\n",numBitmaps);
	
=======
>>>>>>> 10affc5621ee9e9fbccbb07f1f861c6bc54f54e8
	return s->handle;
}

void* bmemalloc(int handler, long n_bytes) {
	int i;
	/* 512 is the max number of allocators we can have at once */
	struct space* s = spaces[handler];
<<<<<<< HEAD
=======
	 
>>>>>>> 10affc5621ee9e9fbccbb07f1f861c6bc54f54e8
	int currentBlockSize = s->minPageSize;
	for (i = s->numBitmaps - 1; i >= 0; i--) {
		/* Find out if n bytes would fit into our current block lvl */
		if (n_bytes <= currentBlockSize)
			break;
		else
			/* prepare to iterate over the next level, with double the block size */
			currentBlockSize *= 2;
	}
<<<<<<< HEAD

=======
	
>>>>>>> 10affc5621ee9e9fbccbb07f1f861c6bc54f54e8
	/* local map */
	char* map = s->bitmaps[i];
	
	/* +1 because we dont have a map of a single item  */
	int lengthOfMap = 1 << (i + 1);
<<<<<<< HEAD
=======
	
>>>>>>> 10affc5621ee9e9fbccbb07f1f861c6bc54f54e8
	int j;
	for (j = 0; j < lengthOfMap; j++) {
		/* find first free */
		if (map[j] == 0) {
			int side = j % 2;
			int myBuddy;
			
			/* this is the left buddy */
			if (side == 0)
				myBuddy = j + 1;
			/* right buddy */
			else if (side == 1)
				myBuddy = j - 1;
				
			/* My buddy better be in use or i can't use this slot */
			if (map[myBuddy] == 1) {
			
				//ALLOCATE HERE
				void* slot = s->head + (currentBlockSize * j);
				
				//Change our bitmap
				map[j] = 1;
				
				return slot;
			
			}
			else {
				/* we are now going to look at bigger blocks */
				break;
			}
		}
	}
<<<<<<< HEAD
	
=======

>>>>>>> 10affc5621ee9e9fbccbb07f1f861c6bc54f54e8
	if (j == lengthOfMap)
		return NULL;;
	
	/* if we are still here, it means we cant find a free slot at the lowest level,
	   it is now necessary to recurr and split some blocks up */
	
	allocAndMark(s, n_bytes, s->bitmaps, i-1, j, currentBlockSize * 2);
<<<<<<< HEAD

=======
	
>>>>>>> 10affc5621ee9e9fbccbb07f1f861c6bc54f54e8
	return NULL;
}

/*
Ok, I need look at the bitmaps from bottom up;
for map in bitmaps:
	for each in map:
		if head + (i * blocksize):
			free;
*/
void bmemfree(void* region) {
	int i;
	/* capture the space where region resides */
	for (i = 0; i < NUM_SPACES; i++) {
		if (region > spaces[i]->head && region < spaces[i]->end) {
			struct space* s = spaces[i];
			
			/* we have found the space that our region is in */
			int currentBlockSize = s->minPageSize;
			
			int j;
			/* look from the lowest bitmap first */
			for (j = s->numBitmaps - 1; j >= 0; j--) {
				// for our bit map, look at each block and see if our region starts there.
				char* map = s->bitmaps[j];
				int lengthOfMap = 2 ^ j;
				int k;
				for (k = 0; k < lengthOfMap -1; k++) {
					/* if  region is found and it's marked as 1 */
					if (s->head + (k * currentBlockSize) == region && map[k] == 1) {
						/* We have indentified the region, and can now free */
						
						freeAndMergeBuddies(s->bitmaps, j, k);
					}
				}
				/*ready to ready the next map */
				currentBlockSize *= 2;
			}
		}
		/* nothing happens in this case */
	}
<<<<<<< HEAD
}
=======
}
>>>>>>> 10affc5621ee9e9fbccbb07f1f861c6bc54f54e8
