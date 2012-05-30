#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "space.h"

#define BFLAG 0x01
#define SFLAG 0x02
#define FFLAG 0x04
#define ALLOC_FLAGS (BFLAG | SFLAG | FFLAG)

struct space* spaces[NUM_SPACES];
int isSpacesInit = 0;

// TEST

// END TEST

int meminit (long n_bytes, unsigned int flags, int parm1, int *parm2){
    int handle;
    int i;
    if (isSpacesInit == 0) {
        isSpacesInit = 1;
        //spaces = (struct space**) malloc (sizeof (struct space*) * NUM_SPACES);
        for (i = 0; i < NUM_SPACES; i++) {
            spaces[i] = NULL;
        }
    }
    
    /* Get an empty space */
    handle = -1;
    for (i = 0; i < NUM_SPACES; i++) {
        if (spaces[i] == NULL) {
            handle = i;
            break;
        }
    }
    
    if (handle == -1) {
        // No more allocators allowed. Error exit.
        return handle;
    }

    /* dispatch to the proper init based on flags */
    switch (flags & ALLOC_FLAGS){
        case BFLAG:
            //buddy
            handle = bmeminit(handle, n_bytes, flags, parm1, parm2);
	    break;
        case SFLAG:
            //slab
            handle = smeminit(handle, n_bytes, flags, parm1, parm2);
	    break;
        case FFLAG:
            //free list first fit
            handle = fmeminit(handle, n_bytes, flags, parm1, parm2);
	    break;
        default:
            //flag not used
            return -1;
	    break;
    }
    return handle;
}


void *memalloc(int handle, long n_bytes){
    char type;
    void *pointer;
    if (handle < 0 || handle >= NUM_SPACES)
        return NULL;
    if (spaces[handle] != NULL){
	    type = spaces[handle]->type;
    }
    else{
	    return NULL;
    }
   
    switch (type){
        case 'b':
            //buddy allocation
            pointer = bmemalloc(handle, n_bytes);
	    break;
        case 's':
            //slab allocation
            pointer = smemalloc(handle, n_bytes);
	    break;
        case 'f':
            //free list allocation
            pointer = fmemalloc(handle, n_bytes);
	    break;
        default:
            //no allocation
            pointer = NULL;
    }
    return pointer;
}


void memfree (void *region){
    struct space* s;
    int i;
    for(i = 0; i < NUM_SPACES; i++){
        if((region >= spaces[i]->head) && (region <= spaces[i]->end)){
            s = spaces[i];
            break;
        }
    }

    if (i == NUM_SPACES) {
        //ERROR
        return;
    }
    
    switch (s->type){
        case 'b':
            bmemfree (s, region);
	    break;	
        case 's':
            smemfree (s, region);
	    break;
        case 'f':
            fmemfree (s, region);
	    break;
        default:
            //do nothing
	    break;
    }
}
