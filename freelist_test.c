#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "space.h"

#define SINIT( _a, _b, _c ) meminit ((_a), 0x02, (_b), (_c))
#define BINIT( _a, _b ) meminit ((_a), 0x01, (_b), NULL)
#define FINIT( _a, _b ) meminit ((_a), (0x04 | _b), NULL, NULL)

int main () {
    int fListTypes[4] = {0x00, 0x08, 0x10, 0x18};
    int fHandles[4];
    int dataSizes[6] = {8, 128, 128, 128, 1024, 4096};
    void* allocated[6];
    int i;
    int j;
    for (i = 0; i < 4; i++) {
        printf ("meminit: n_bytes = %d, list type = %hd\n", 4096, fListTypes[i]);
        fHandles[i] = FINIT (4096, fListTypes[i]);
        printf ("   return: handle = %d\n", fHandles[i]);
        printf ("allocation:\n");
        for (j = 0; j < 6; j++) {
            printf ("  memalloc: size = %d, handle = %d\n", dataSizes[j], fHandles[i]);
            allocated[j] = memalloc (fHandles[i], dataSizes[j]);
            printf ("      return: ptr = %p\n", allocated[j]);
            if (allocated[j] != NULL) {
                printf ("  memfree: region = %p\n", allocated[j]);
                memfree (allocated[j]);
                printf ("      return\n");
            }
        }
    }
}
