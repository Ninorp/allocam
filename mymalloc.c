#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void *global_pointer = NULL;

typedef struct chunk Chunk;
struct chunk {
    size_t size;
    struct chunk *next;
    int free;
};

#define CHUNK_SIZE sizeof(Chunk)

int MyMallocInit(int size) {
    void *request = sbrk(size);
    if(request == (void*) -1)
        return 0;
    return 1;
}

Chunk *find_free_chunk(Chunk** last, size_t size) {
    Chunk *current = global_pointer;
    while(current && !(current->free && current->size >= size)) {
        *last = current;
        current = current->next;
    }
    return current;
}

Chunk *request_space(Chunk* last, size_t size) {
    Chunk* block;
    block = sbrk(0);
    void *request = sbrk(size + CHUNK_SIZE);
    assert((void*) block == request);
    if(request == (void*) - 1) {
        return NULL;
    }
    if(last) {
        last->next = block;
    }
    block->size = size;
    block->next = NULL;
    block->free = 0;
    return block;
}

void split_chunk(Chunk *block, size_t size) {
    Chunk *new;
    new->size = block->size - CHUNK_SIZE - size;
    new->next = block->next;
    new->free = 1;
    block->size = size;
    block->next = new;
}

void *allocam(size_t size) {
    Chunk *block;
    if(size <= 0) 
        return NULL;
    if(!global_pointer) {
        block = request_space(NULL, size);
        if(!block)
            return NULL;
    }
    else {
        Chunk *last = global_pointer;
        block = find_free_chunk(&last, size);
        if(!block) {
            block = request_space(last, size);
            if(!block)
                return NULL;
        } else {
            if (size < block->size) split_chunk(block, size);
            block->free = 0;
        }
    }
    return(block + 1); // nós queremos retornar o ponteiro para a regiao após o metadata do bloco
}

Chunk *get_chunk_pointer(void *pointer) {
    return (Chunk*) pointer - 1;
}

void freem(void *pointer) {
    if(!pointer)
        return;
    Chunk *chunk_pointer = get_chunk_pointer(pointer);
    assert(chunk_pointer->free == 0);
    chunk_pointer->free = 1;
}

int main(void) {
    for(int i = 0; i < 10; i++) {
        void *p = allocam(i);
        printf("%p\n", (void *) p);
    }
    return 0;
}