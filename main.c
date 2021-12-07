
#include <stdlib.h>
#include <stdio.h>
#include "lib/mymalloc.h"
#include "test/test.h"

int main(void) {
    int choice;
    printf("1 - Testar Tempo | 2 - Testar Performance | 3 - Testar Fragmentação (MyMalloc) \n");
    scanf("%d", &choice);
    switch(choice) {
        case 1:
            test_time_malloc();
            test_time_mymalloc();
            break;
        case 2:
            test_performance_malloc();
            test_performance_mymalloc();
            break;
        case 3:
            test_fragmentation_mymalloc();
            break;
    }
    return 0;
}