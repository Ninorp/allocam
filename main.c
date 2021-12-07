
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include "lib/mymalloc.h"
#include "test/test.h"

int main(void) {
    test_time_malloc();
    test_time_mymalloc();
    test_performance_malloc();
    test_performance_mymalloc();
    test_fragmentation_mymalloc();

    return 0;
}