#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "test.h"
#include "../lib/mymalloc.h"

void test_time_malloc() {
    printf("Testando Tempo - Malloc\n");
    gettimeofday(&tval_before, NULL);

    for (size_t j = 1000; j <= 100000; j += 1000) {
        int *inteiros = (int *) malloc(sizeof(int) * j);
        for (size_t i = 1; i < j; i++) {
            inteiros[i] = 0;
        }
        for (size_t i = 1; i < j; i++) {
            assert(inteiros[i] == 0);
        }
    }
    
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    printf("Malloc - Tempo decorrido: %ld.%06ld seconds\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
}

void test_time_mymalloc() {
    printf("Testando Tempo - MyMalloc\n");
    gettimeofday(&tval_before, NULL);

    for (size_t j = 1000; j <= 100000; j += 1000) {
        int *inteiros = (int *) mymalloc(sizeof(int) * j);
        for (size_t i = 1; i < j; i++) {
            inteiros[i] = 0;
        }
        for (size_t i = 1; i < j; i++) {
            assert(inteiros[i] == 0);
        }
    }
    
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    printf("MyMalloc - Tempo decorrido: %ld.%06ld seconds\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
}

void test_performance_malloc() {
    printf("Testando Performance - Malloc\n");
    gettimeofday(&tval_before, NULL);

    for (size_t j = 1000; j <= 100000; j += 1000) {
        int *inteiros = (int *) malloc(sizeof(int) * j);
        free(inteiros);
    }
    
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    printf("MyMalloc - Tempo decorrido: %ld.%06ld seconds\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
}

void test_performance_mymalloc() {
    printf("Testando Performance - MyMalloc\n");
    gettimeofday(&tval_before, NULL);

    for (size_t j = 1000; j <= 100000; j += 1000) {
        int *inteiros = (int *) mymalloc(sizeof(int) * j);
        myfree(inteiros);
    }
    
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);
    printf("MyMalloc - Tempo decorrido: %ld.%06ld seconds\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
}