#include <stdio.h>
#include <stdlib.h>
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

int main () {
    printf ("smeminit:\n");
    int parm2[5] = {5, 50, 64, 75, 0};
    int sHandle = inits (4096 * 8, 2, parm2);
    printSpace (sHandle);
    printf ("bmeminit:\n");
    int bHandle = initb (0x1000, 8);
    printSpace (bHandle);
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
}
