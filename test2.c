#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "space.h"

#define inits( _a, _b, _c ) meminit ((_a), 0x02, (_b), (_c))
#define initb( _a, _b ) meminit ((_a), 0x01, (_b), NULL)
#define initf( _a, _b ) meminit ((_a), (0x04 | _b), NULL, NULL)

void printSpace (int handle) {
    struct space* s = spaces[handle];

    printf ("***spaces[%d]***\n", handle);
    printf ("general:\n");
    printf ("->type = %c\n->handle = %d\n->head = %p\n->end = %p\n->size = %d\n",
                s->type, s->handle, s->head, s->end, s->size);
    printf ("buddy & slab:\n");
    printf ("->minPageSize = %d\n->numBitmaps = %d\n->bitmaps = %p\n",
                s->minPageSize, s->numBitmaps, s->bitmaps);
    printf ("freelist:\n");
    printf ("->listType = %d\n->firstFree = %p\n->nextFree = %p\n",
                s->listType, s->firstFree, s->nextFree);
    printf ("slab:\n");
    printf ("->sizeArray = %p\n->numSlabs = %d\n->slabs = %p\n->slabSize = %d\n",
                s->sizeArray, s->numSlabs, s->slabs, s->slabSize);
    printf ("\n");
}

void showMeTheBitMapPlease(int handle) {
	struct space* s = spaces[handle];
	int i;
	int currMapLen = 1;
	for (i = 0 ; i < s->numBitmaps; i++) {
		int j;
		printf("Bitmap[%d]: ", i);
		for (j = 0; j < currMapLen; j++) {
			printf ("%d", s->bitmaps[i][j]);
		}
		printf("\n");
		currMapLen *= 2;
	}
}

int main() {
	
}