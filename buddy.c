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

void markDown(long n_bytes, char** maps, int mapIdx, int blockIdx, int blockSize) {
	if (n_bytes <= blockSize) {
		maps[mapIdx][blockIdx] = 1;
		markDown(n_bytes, maps, mapIdx + 1, blockIdx * 2, blockSize / 2);
	}
}

void* allocAndMark(struct space* s, long n_bytes, char** maps, int mapIdx, int blockIdx, int blockSize) {
	printf("\nmaps: %p\n", maps);
	int mapSize = 1 << (mapIdx + 1);
	int j;
	printf("blockIdx(j): %d\n", blockIdx);
	printf("mapSize: %d\n", mapSize);
	for (j = blockIdx; j < mapSize; j++) {
		/* we try to allocate at the current level, figure out which buddy we are*/
		int buddy;
		
		printf("j: %d\n", j);
		if (j % 2 == 0)
			buddy = j + 1;
		else
			buddy = j - 1;
			
		printf("   buddy: %p\n", buddy);
		/* we have an in-use buddy, we can allocate */
		printf("   mapIdx: %d\n", mapIdx);
		printf("   maps[mapIdx]: %p\n", maps[mapIdx]);
		printf("   maps[mapIdx][buddy]: %p\n", maps[mapIdx][buddy]);
		if (maps[mapIdx][buddy] == 1) {
			maps[mapIdx][j] = 1;
			
			/* TODO: resolve the block size somehow */
			void* slot = s->head + (blockSize * j);
			
			/* WE need to travel back down 
			*/
			markDown(n_bytes, maps, mapIdx + 1, j * 2, blockSize / 2);
			
			return slot;
			
		}
		else if (mapIdx >= 1) {
			return allocAndMark(s, n_bytes, maps, mapIdx - 1, j / 2, blockSize * 2);
		}
	}
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
	
	/* if n_bytes is not a power of two, then there's no way to continue. */
	if (!isPowerOfTwo(n_bytes))
		return -1;
	
	/* This is the maximum number of pages we can get from n bytes, where parm1 is the minimum size of a page */
	int minPageSize = 1 << (parm1); //parm1 min page size in bits
	int numPages = n_bytes / minPageSize;
	
	if (numPages < 1)
		return -1;
	
	/* calculating number of Bitmaps we need */
	int numBitmaps = 0;
	int tmp;
	for(tmp = numPages; tmp > 0; tmp = tmp >> 1) {
		numBitmaps += 1;
	}
	
	/* Since we can only call malloc once, this is the total space we need */
	int initMapLength = 1;
	int mapsSize = 0;
	for (i = 0; i < numBitmaps; i++) {
		mapsSize += sizeof(char) * initMapLength;
		initMapLength *= 2;
	}
	
	size_t totalSpace = sizeof(struct space) + sizeof(void*) * numBitmaps + mapsSize + n_bytes;
	
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
	
	/* Now loop over and create maps of size 1 and greater*/
	for (i = 0; i < numBitmaps; i++) {
		s->bitmaps[i] = tmp_front;
		
		/*clear bitmap*/
		memset(s->bitmaps[i], 0, sizeof(char) * initMapLength);
		
		tmp_front += sizeof(char) * initMapLength;
		initMapLength *= 2;
	}
	
	/* THIS IS WHERE ACTUAL DATA GOES */
    s->type = 'b';
    
	s->head = tmp_front;
	
	s->end = s->head + n_bytes - 1;
	
	s->size = n_bytes;
	
	s->minPageSize = minPageSize;
	
	/* we need to know how many bitmaps we have */
	s->numBitmaps = numBitmaps;
	
	s->firstFree = NULL;
	
	/* assign a handler to our space struct*/
	
	s->handle = handle;
	spaces[s->handle] = s;
	
	
	return s->handle;
}
/* return index of first zero */
int findFreePage(char* map, int mapSize) {
	int i = 0;
	for (i = 0; i < mapSize; i++) {
		if (map[i] == 0)
			return i;
	}
	return -1;
}

int getBuddyIndex(int index) {
	if (index % 2 == 0)
		return index + 1;
	else
		return index - 1;
}

void* bGetPointer (struct space* s, int mapIdx, int blockIdx) {
	void* region = s->head;
	int rSize = s->minPageSize << (s->numBitmaps - mapIdx - 1);
	region += rSize * blockIdx;
	return region;
}

void* bmemalloc(int handler, long n_bytes) {
	struct space* s = spaces[handler];
	
	int bm;
	int fitSize = s->minPageSize;
	for (bm = s->numBitmaps - 1; bm >= 0; bm--) {
		if (fitSize >= n_bytes)
			break;
		fitSize *= 2;
	}
	
	char** maps = s->bitmaps;
	
	int index = findFreePage(maps[bm], 1 << bm);
	
	/* If the buddy is allocated, our life is soooo easy */
	if (bm != 0) {
		int buddyIndex = getBuddyIndex (index);
		if (maps[bm][buddyIndex] == 1) {
			maps[bm][index] = 1;
			return bGetPointer (s, bm, index);
		}
	} else {
		if (maps[bm][0] == 0)
			return bGetPointer (s, bm, 0);
	}
	
	/* Find a level with an allocated buddy */
	int freeLevel;
	for (freeLevel = bm - 1; freeLevel >= 0; freeLevel--) {
		index = findFreePage(maps[freeLevel], 1 << freeLevel);
		if (index == -1)
			return NULL;
		if (freeLevel == 0 || (maps[bm][getBuddyIndex(index)] == 1))
			break;
	}
	
	/* Now we found a free level to split, go back down marking the bit as 1 until the proper level is hit */
	int i;
	for (i = freeLevel; i <= bm; i++) {
		maps[i][index] = 1;
		index *= 2;
	}
	
	return bGetPointer (s, bm, index);
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
}
