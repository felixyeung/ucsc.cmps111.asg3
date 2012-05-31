#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "space.h"

#define inits( _a, _b, _c ) meminit ((_a), 0x02, (_b), (_c))
#define initb( _a, _b ) meminit ((_a), 0x01, (_b), NULL)
#define initf( _a, _b ) meminit ((_a), (0x04 | _b), NULL, NULL)

void printWaste(int handle) {
	struct space* s = spaces[handle];
	printf("======\n======>%f used total, %f managed.\n======\n", ((void*)s->end - (void*)s), s->size);
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

int main () {
    printf ("smeminit:\n");
    int parm2[5] = {5, 50, 64, 75, 0};
    int sHandle = inits (4096 * 128, 2, parm2);
	printWaste(sHandle);
    printSpace (sHandle);
    printf ("bmeminit:\n");
    int bHandle = initb (0x1000, 8);
    printSpace (bHandle);
	showMeTheBitMapPlease(bHandle);
    printf ("fmeminit:\n");
    printf ("first fit:\n");
    int ffHandle = initf (10000, 0x00);
    printSpace (ffHandle);
    printf ("next fit:\n");
    int nfHandle = initf (10000, 0x08);
    printSpace (nfHandle);
    printf ("best fit:\n");
    int bfHandle = initf (10000, 0x10);
    printSpace (bfHandle);
    printf ("worst fit:\n");
    int wfHandle = initf (10000, 0x18);
    printSpace (wfHandle);
	
	printf("now let's try to allocate a string into all three free slots: \n");
	
	printf("SOURCE: \n");
	char* myString = "This is my string.";
	printf("myString: %s\n", myString);
	char* myString2 = "This is my second string.";
	printf("myString2: %s\n", myString2);
	char* myString3 = "This is my third string!";
	printf("myString3: %s\n", myString3);
	
	printf("TARGETS:\n");
	char* slabString =  (char*)memalloc(sHandle, strlen(myString) + 1);
	strcpy(slabString, myString);
	printf("slabString: %s\n", slabString);
	char* slabString2 =  (char*)memalloc(sHandle, strlen(myString2) + 1);
	strcpy(slabString2, myString2);
	printf("slabString2: %s\n", slabString2);
	char* slabString3 =  (char*)memalloc(sHandle, strlen(myString3) + 1);
	strcpy(slabString3, myString3);
	printf("slabString3: %s\n", slabString3);
	
	char* freelistString =  (char*)memalloc(ffHandle, strlen(myString) + 1);
	strcpy(freelistString, myString);
	printf("first freelistString: %s\n", freelistString);
	char* freelistString2 =  (char*)memalloc(ffHandle, strlen(myString2) + 1);
	strcpy(freelistString2, myString2);
	printf("first freelistString2: %s\n", freelistString2);
	char* freelistString3 =  (char*)memalloc(ffHandle, strlen(myString3) + 1);
	strcpy(freelistString3, myString3);
	printf("first freelistString3: %s\n", freelistString3);
	
	freelistString =  (char*)memalloc(nfHandle, strlen(myString) + 1);
	strcpy(freelistString, myString);
	printf("next freelistString: %s\n", freelistString);
	
	freelistString =  (char*)memalloc(bfHandle, strlen(myString) + 1);
	strcpy(freelistString, myString);
	printf("best freelistString: %s\n", freelistString);
	
	freelistString =  (char*)memalloc(wfHandle, strlen(myString) + 1);
	strcpy(freelistString, myString);
	printf("worst freelistString: %s\n", freelistString);

	char* buddyString =  (char*)memalloc(bHandle, strlen(myString) + 1);
	strcpy(buddyString, myString);
	printf("buddyString: %s\n", buddyString);
	char* buddyString2 =  (char*)memalloc(bHandle, strlen(myString2) + 1);
	strcpy(buddyString2, myString2);
	printf("buddyString2: %s\n", buddyString2);
	char* buddyString3 =  (char*)memalloc(bHandle, strlen(myString3) + 1);
	strcpy(buddyString3, myString3);
	printf("buddyString3: %s\n", buddyString3);
	
	printf("Now, lets try to free our things: \n");
	
	memfree(slabString);
	printf("   freeing slab: [%s]\n", slabString);
	memfree(buddyString);
	printf("   freeing buddy (%p): [%s]\n", buddyString, slabString);
	memfree(buddyString2);
	printf("   freeing buddy (%p): [%s]\n", buddyString2, buddyString2);
	/*
	memfree(freelistString);
	printf("   freeing freelist: [%s]\n", freelistString);
	*/
}
