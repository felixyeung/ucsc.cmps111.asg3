#include <stdlib.h>
#include <stdio.h>

int main () {
	//Testing Buddy
	printf("Testing Buddy\n");
	int bHandle = meminit (512, 0x01, 4, NULL);
    printf ("bmeminit: %d\n", bHandle);
	void* ptrb = memalloc (sHandle, 4);
    printf ("allocated %p\n", ptr1);
	free(ptrb);
	printf ("Freed ptr\n");
	int bHandle = meminit (512, 0x01, 3, NULL);
    printf ("bmeminit: %d\n", bHandle);
	void* ptrb = memalloc (sHandle, 4);
    printf ("allocated %p\n", ptr1);
	free(ptrb);
	printf ("Freed ptr\n");
	
	//Testing Slab
	printf("Testing Slab\n");
    int foo[3] = {5, 3, 0};
    int sHandle = meminit (16384, 0x02, 2, foo);
    printf ("smeminit: %d\n", sHandle);
	void* ptrs = memalloc (sHandle, 3);
    printf ("allocated %p\n", ptrs);
	free(ptrs);
	printf ("Freed ptr\n");
    int bar[5] = {50, 200, 100, 25, 0};
    int sHandle = meminit (50000, 0x02, 3, bar);
    printf ("smeminit: %d\n", sHandle2);
   	void* ptrs = memalloc (sHandle, 3);
    printf ("allocated %p\n", ptrs);
   	free(ptrs);
	printf ("Freed ptr\n");
	
	//Test Free
	printf("Testing Free-list\n");
	int fHandle = meminit (100, 0x04, NULL, NULL);
    printf ("ffmeminit: %d\n", fHandle);
	void* ptrf = memalloc (sHandle, 10);
    printf ("allocated %p\n", ptrs);
	free(ptrf);
	printf ("Freed ptr\n");
	
	int fHandle = meminit (100, 0x0A, NULL, NULL);
    printf ("fnmeminit: %d\n", fHandle);
   	void* ptrf = memalloc (sHandle, 10);
    printf ("allocated %p\n", ptrs);
	free(ptrf);
	printf ("Freed ptr\n");
	
 	int fHandle = meminit (100, 0x10, NULL, NULL);
    printf ("bfmeminit: %d\n", fHandle);
	void* ptrf = memalloc (sHandle, 10);
    printf ("allocated %p\n", ptrs);
	free(ptrf);
	printf ("Freed ptr\n"); 
 
 	int fHandle = meminit (100, 0x1A, NULL, NULL);
    printf ("wfmeminit: %d\n", fHandle);
	void* ptrf = memalloc (sHandle, 10);
    printf ("allocated %p\n", ptrs);
	free(ptrf);
	printf ("Freed ptr\n");
	
    return 0;
}
