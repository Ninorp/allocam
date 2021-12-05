#include <stdio.h>
#include "mymalloc.h"

int main(void) {
    int *a = mymalloc(4* sizeof(int));
    int *b = mymalloc(4* sizeof(int));
    mymallocgerency();
    int *c = mymalloc(sizeof(int));
    myfree(a);
    return 0;
}