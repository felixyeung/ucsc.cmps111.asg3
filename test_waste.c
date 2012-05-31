#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "space.h"

#define inits( _a, _b, _c ) meminit ((_a), 0x02, (_b), (_c))
#define initb( _a, _b ) meminit ((_a), 0x01, (_b), NULL)
#define initf( _a, _b ) meminit ((_a), (0x04 | _b), NULL, NULL)

void printWaste(int handle) {
	struct space* s = spaces[handle];
	printf("   ============================================\n");
	printf("         %d used total, %d managed.\n", (int)((void*)s->end - 1 - (void*)s), s->size);
	printf("   ============================================\n");
}

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
	int i;
	int parm2[5] = {5, 50, 64, 75, 0};
	for (i = 1; i < 10; i ++) {
		printf("INIT %d BYTES (%d * 4096 for SLAB)\n", 128 << i, 128 << i);
			int sHandle = inits (4096 * (128 << i), 2, parm2);
			int bHandle = initb (128 << i, 8);
			int ffHandle = initf (128 << i, 0x00);
			printf(" Slab\n");
			printWaste(sHandle);
			printf(" Buddy\n");
			printWaste(bHandle);
			printf(" Freelist\n");
			printWaste(ffHandle);
	}
}