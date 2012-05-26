int meminit (long n_bytes, unsigned int flags, int parm1, int *parm2){
    int handle;
    if (isSpacesInit == 0){
        isSpacesInit = 1;
        memset(spaces,0,512);
    }

    /* dispatch to the proper init based on flags */
    switch (flags){
        case 0x01:
            //buddy
            handle=bmeminit(n_bytes, flags, parm1, parm2);
	    break;
        case 0x02:
            //slab
            handle=smeminit(n_bytes, flags, parm1, parm2);
	    break;
        case 0x04:
            //free list first fit
            handle=fmeminit(n_bytes, 0x00, parm1, parm2);
	    break;
        case 0x0E:
            //free list next fit
            handle=fmeminit(n_bytes, 0x08, parm1, parm2);
	    break;
        case 0x14:
            //free list best fit
            handle=fmeminit(n_bytes, 0x10, parm1, parm2);
	    break;
        case 0x1E:
            //free list worst fit
            handle=fmeminit(n_bytes, 0x18, parm1, parm2);
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
