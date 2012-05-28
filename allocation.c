#include <stdlib.h>
#include "space.h"

#define BFLAG 0x01
#define SFLAG 0x02
#define FFLAG 0x04
#define ALLOC_FLAGS (BFLAG | SFLAG | FFLAG)

int meminit (long n_bytes, unsigned int flags, int parm1, int *parm2){
    int handle;
    if (isSpacesInit == 0){
        isSpacesInit = 1;
        memset(spaces,0,512);
    }
    
    /* Get an empty space */
    int i;
    int handle = -1;
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
            handle=bmeminit(handle, n_bytes, flags, parm1, parm2);
	    break;
        case SFLAG:
            //slab
            handle=smeminit(handle, n_bytes, flags, parm1, parm2);
	    break;
        case FFLAG:
            //free list first fit
            handle=fmeminit(handle, n_bytes, flags, parm1, parm2);
	    break;
        default:
            //flag not used
            handle=-1;
	    break;
    }
    return handle;
}


void *memalloc(int handle, long n_bytes){
    char type;
    void *pointer;
    if (spaces[handle] != NULL){
	type=spaces[handle]->type;
    }
    else{
	 return NULL;
    }

   
    switch (type){
        case b:
            //buddy allocation
            pointer=bmemalloc(handle, n_bytes);
	    break;
        case s:
            //slab allocation
            pointer=smemalloc(handle, n_bytes);
	    break;
        case f:
            //free list allocation
            pointer=fmemalloc(handle, n_bytes);
	    break;
        default:
            //no allocation
            pointer = NULL;
    }
    return pointer;
}


void memfree (void *region){
    char type;
    int i;
    for(i = 0; i < 512; i++){
	if((region > spaces[i]->head) && (region < spaces[i]->end)){
		type=spaces[i]->type;
		break;
	}
    }

    switch (type){
        case b:
            bmemfree(region);
	    break;	
        case s:
            smemfree(region);
	    break;
        case f:
            fmemfree(region);
	    break;
        default:
            //do nothing
	    break;
    }
}
