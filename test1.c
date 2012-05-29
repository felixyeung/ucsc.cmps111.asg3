#include <stdlib.h>
#include <stdio.h>

int main () {
    int foo[3] = {1, 3, 0};
    int sHandle = meminit (100, 0x02, 2, foo);
    printf ("smem: %d\n", sHandle);
    int fHandle = meminit (100, 0x04, NULL, NULL);
    printf ("fmem: %d\n", fHandle);
    int bHandle = meminit (512, 0x01, 4, NULL);
    printf ("bmem: %d\n", bHandle);
    return 0;
}
