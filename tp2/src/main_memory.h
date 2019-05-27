#ifndef MAIN_MEMORY_H
#define MAIN_MEMORY_H

#define MAIN_MEMORY_SIZE 		64 * 1024 // 64KB
#define MAIN_MEMORY_BLOCK_SIZE	64 // 64B
#define MAIN_MEMORY_BLOCKS		( 64 * 1024 ) / 64

typedef struct main_memory_block {
    char* data;
} main_memory_block_t;

typedef struct main_memory {
    int size;
    main_memory_block_t* blocks[MAIN_MEMORY_BLOCKS];
    size_t blocks_count;
} main_memory_t;

extern main_memory_t* MAIN_MEMORY;

/* Initializes the main memory blocks as invalid and the rate of misses to 0. */
void init_main_memory();

/* Destroys the Main Memory*/
void destroy_main_memory();

#endif // MAIN_MEMORY_H
