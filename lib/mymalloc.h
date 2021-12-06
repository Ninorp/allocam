struct chunk {
    size_t size;
    struct chunk *next;
    struct chunk *prev;
    void *pointer;
    int free;
    char data[1];
};

#define BLOCK_SIZE (sizeof(struct chunk) - 1)
#define FRAGM_LIMIT 2

typedef struct chunk *block;

void split_block(block b, size_t size);
block find_block(block *last, size_t size);
block extend_heap(block last, size_t size);
block get_block(void *pointer);
int verify_valid_addr(void *pointer);
block merge_blocks(block b);
void *mymalloc(size_t size);
void merge_all();
void myfree(void *pointer);
void mymallocgerency();