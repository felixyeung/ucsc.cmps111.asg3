#include <stdio.h>
#include <stdlib.h>

#define initslab( _a, _b, _c ) meminit ((_a), 0x02, (_b), (_c))

int main () {
    printf ("smeminit:\n");
    int parm2[5] = {5, 50, 64, 75, 0}
    int sHandle = initslab (4096 << 4, 2, parm2);
}
