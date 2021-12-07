#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "mymalloc.h"

void *global_pointer = NULL;
int global_free_count = 0;

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
            global_free_count--;
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
        if(b->next) {
            b->next->prev = b;
        }
        global_free_count--;
    }
    return b;
}

void copy_data(block source, block dest) {
    int *source_data, *dest_data;
    size_t i;
    source_data = source->pointer;
    dest_data = dest->pointer;
    for(i = 0; i < source->size && i < dest->size; i++) {
        dest_data[i] = source_data[i];
    }
}

void merge_all() {
    block b = (block) global_pointer;
    while(global_free_count >= FRAGM_LIMIT) {
        if(b->free) {
            block hold_next;
            if(b->next->next)
                hold_next = b->next->next;
            block sup = b->next;
            size_t hold_size = b->size;
            b->size = b->next->size;
            b->free = b->next->free;
            copy_data(b->next, b);
            sup = b + BLOCK_SIZE + b->size;
            sup->free = 1;
            sup->size = hold_size;
            sup->next = hold_next;
            sup->prev = b;
            sup->pointer = sup->data;
            b->next = sup;
        }
        b = b->next;
        if(b->next->free) {
            merge_blocks(b);
        }
    }
}

void myfree(void *pointer) {
    if (verify_valid_addr(pointer)) {
        block b = get_block(pointer);
        b->free = 1;
        global_free_count++;
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
            global_free_count--;
        }

        if(global_free_count >= FRAGM_LIMIT) {
            merge_all();
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

void freecountgerency() {
    printf("%d\n", global_free_count);
}