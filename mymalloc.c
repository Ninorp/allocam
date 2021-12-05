#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

struct chunk {
    size_t size;
    struct chunk *next;
    struct chunk *prev;
    void *pointer;
    int free;
    char data[1];
} __attribute__((packed));

#define BLOCK_SIZE (sizeof(struct chunk) - 1)

typedef struct chunk *block;

void *global_pointer = NULL;

void split_block(block b, size_t s) {
    block new;
    new = (block)(b->data + s);
    new->size = b->size - s - BLOCK_SIZE;
    new->next = b->next;
    new->prev = b;
    new->free = 1;
    new->pointer = new->data;
    b->size = s;
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

block extend_heap(block last, size_t s) {
    block b = (block) sbrk(0);
    if(sbrk(BLOCK_SIZE + s) == (void *) -1) {
        return NULL;
    }
    b->size = s;
    b->next = NULL;
    b->prev = last;
    b->pointer = b->data;
    if(last) {
        last->next = b;
    } 
    b->free = 0;
    return b;
}

void *malloc(size_t size) {
    block b;
    if(global_pointer) {
        block last = (block) global_pointer;
        b = find_block(&last, size);
        if(b) {
            if((b->size - size) >= (BLOCK_SIZE))
                split_block(b, size);
            b->free = 0;
        } else {
            b = extend_heap(last, size);
            if(!b)
                return NULL;
        }
    } else {
        b = extend_heap(NULL, size);
        if(!b)
            return NULL;
        global_pointer = b;
    }
    return b->data;
}

block get_block(void *p) {
    char *tmp;
    tmp = p;
    return (p = tmp -= BLOCK_SIZE);
}

int valid_addr(void *p) {
    if(global_pointer) {
        if(p > global_pointer && p < sbrk(0)) {
            block test = get_block(p);
            return p == (get_block(p)->pointer);
        }
    }
    return 0;
}

block fusion(block b) {
    if(b->next && b->next->free) {
        b->size += BLOCK_SIZE + b->next->size;
        b->next = b->next->next;
        if(b->next)
            b->next->prev = b;
    }
    return b;
}

void free(void *p) {
    if (valid_addr(p)) {
        block b = get_block(p);
        b->free = 1;
        if(b->prev && b->prev->free) {
            b = fusion(b->prev);
        }
        if(b->next) {
            fusion(b);
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

void MyMallocGerency() {
    printf("------MEMORY SPACES-----\n");
    block b = global_pointer;
    while(b && b->next) {
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

int main(void) {
    int *a = malloc(4* sizeof(int));
    int *b = malloc(4* sizeof(int));
    free(a);
    int *c = malloc(sizeof(int));
    MyMallocGerency();
    return 0;
}