#include <stdlib.h>
#include <stdio.h>

int main () {
    int foo[3] = {5, 3, 0};
    int sHandle = meminit (16384, 0x02, 2, foo);
    printf ("smeminit: %d\n", sHandle);
    int fHandle = meminit (100, 0x04, NULL, NULL);
    printf ("fmeminit: %d\n", fHandle);
    int bHandle = meminit (512, 0x01, 4, NULL);
    printf ("bmeminit: %d\n", bHandle);
    int bar[5] = {50, 200, 100, 25, 0};
    int sHandle2 = meminit (50000, 0x02, 3, bar);
    printf ("smeminit 2: %d\n", sHandle2);
    
    //Allocate some memory
    void* ptr1 = memalloc (sHandle, 3);
    printf ("allocated %p\n", ptr1);
    void* ptr2 = memalloc (sHandle, 3);
    printf ("allocated %p\n", ptr2);
//    memfree (ptr1);
//    printf ("freed %p\n", ptr1);
//    void* ptr3 = memalloc (fHandle, 10);
//    printf ("allocated %p\n", ptr3);
//    void* ptr4 = memalloc (bHandle, 16);
//    printf ("allocated %p\n", ptr4);
//    void* ptr5 = memalloc (sHandle2, 150);
//    printf ("allocated %p\n", ptr5);
//    printf ("%p, %p, %p, %p, %p\n", ptr1, ptr2, ptr3, ptr4, ptr5);
    
    return 0;
}
