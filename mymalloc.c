#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "mymalloc.h"

void *global_pointer = NULL;

void split_block(block b, size_t size) {
    block new;
    new = (block)(b->data + size);
    new->size = b->size - size - BLOCK_SIZE;
    new->next = b->next;
    new->prev = b;
    new->free = 1;
    new->pointer = new->data;
    b->size = size;
    b->next = new;
    if(new->next)
        new->next->prev = new;
}

block find_block(block *last, size_t size) {
    block b = (block) global_pointer;
    while(b && !(b->free && b->size >= size)) {
        *last = b;
        b = b->next;
    }
    return b;
}

block extend_heap(block last, size_t size) {
    block b = (block) sbrk(0);
    if(sbrk(BLOCK_SIZE + size) == (void *) -1) {
        return NULL;
    }
    b->size = size;
    b->next = NULL;
    b->prev = last;
    b->pointer = b->data;
    if(last) {
        last->next = b;
    } 
    b->free = 0;
    return b;
}

void *mymalloc(size_t size) {
    block b;
    if(global_pointer) {
        block last = (block) global_pointer;
        b = find_block(&last, size);
        if(b) {
            if((b->size - size) >= (BLOCK_SIZE)) {
                split_block(b, size);
            }
            b->free = 0;
        } else {
            b = extend_heap(last, size);
            if(!b) {
                return NULL;
            }
        }
    } else {
        b = extend_heap(NULL, size);
        if(!b) {
            return NULL;
        }
        global_pointer = b;
    }
    return b->data;
}

block get_block(void *pointer) {
    char *sup;
    sup = pointer;
    return pointer = sup -= BLOCK_SIZE;
}

int verify_valid_addr(void *pointer) {
    if(global_pointer) {
        if(pointer > global_pointer && pointer < sbrk(0)) {
            return pointer == get_block(pointer)->pointer;
        }
    }
    return 0;
}

block merge_blocks(block b) {
    if(b->next && b->next->free) {
        b->size += BLOCK_SIZE + b->next->size;
        b->next = b->next->next;
        if(b->next)
            b->next->prev = b;
    }
    return b;
}

void myfree(void *pointer) {
    if (verify_valid_addr(pointer)) {
        block b = get_block(pointer);
        b->free = 1;
        if(b->prev && b->prev->free) {
            b = merge_blocks(b->prev);
        }
        if(b->next) {
            merge_blocks(b);
        } else {
            if(b->prev) {
                b->prev->next = NULL;
            } else {
                global_pointer = NULL;
            }
            brk(b);
        }
    }
}

void mymallocgerency() {
    printf("------MEMORY SPACES-----\n");
    block b = global_pointer;
    while(b) {
        printf("%p", (void *) b);
        printf(" - ");
        printf("TAMANHO: %ld BYTES", b->size);
        printf(" - ");
        if(b->free) {
            printf("LIVRE\n");
        } else {
            printf("OCUPADO\n");
        }
        b = b->next;
    }
}