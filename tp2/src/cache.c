#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#include "main_memory.h"
#include "cache.h"

#define WAYS 4
#define CACHE_SIZE 2 * 1024 // 2KB
#define BLOCK_SIZE 64 // 64B
#define BLOCKS (2 * 1024)/(64 * 4) // 8 blocks

#define BITS_TAG 7
#define AND_TAG 65024
#define SHIFT_TAG 9

#define BITS_INDEX 3
#define AND_INDEX 448
#define SHIFT_INDEX 6

#define BITS_OFFSET 6
#define AND_OFFSET 63
#define SHIFT_OFFSET 6

#define BITS_ADDRESS 16

/* -------------------- UTILS --------------------- */

char* int_to_binary(int n, size_t bits) {
	int c, d, count;
	char *pointer;

	count = 0;
	pointer = (char*)malloc(bits + 1);
	if (!pointer) {
		puts("ERROR: Can't Initialize blocks from cache");
		abort();
	}

	for (c = bits - 1; c >= 0; c--) {
		d = n >> c;

		if (d & 1)
			*(pointer+count) = 1 + '0';
		else
			*(pointer+count) = 0 + '0';

		count++;
	}
	*(pointer+count) = '\0';

	return  pointer;
}

int binary_to_int(char* bin, size_t bits) {
	int result = 0;
	int count = bits - 1;
	for (int i = 0; i < bits; ++i) {
		if (bin[i] == '1') {
			result += pow(2, count);
		}
		count--;
	}
	return result;
}

unsigned int get_tag(unsigned int address) {
	return (address & AND_TAG) >> SHIFT_TAG;
}

unsigned int get_index(unsigned int address) {
	return (address & AND_INDEX) >> SHIFT_INDEX;
}

unsigned int get_offset(unsigned int address) {
	return (address & AND_OFFSET);
}

/* -----------------BLOCK ------------------- */

typedef struct block{
	time_t lastUpdate;
	int valid;
	int dirty;
	int tag;
	char* data;
}block_t;

block_t* init_block() {

	block_t* block = malloc(sizeof(block_t));
	if (!block) {
		puts("Error initializing blocks from cache");
		abort();
	}
	
	block->data = malloc(BLOCK_SIZE * sizeof(char*));
	if (!block->data) {
		puts("Error initializing data blocks from cache");
		abort();
	}
	
	block->lastUpdate = time(NULL);
	block->valid = 0;
	block->dirty = 0;

	return block;
}

void destroy_block(block_t* block) {
	if(block) free(block->data);
	free(block);
}

/* ---------------------------------------------------- */

/* ----------------- SET ------------------- */

typedef struct cache_set{
	block_t* blocks[BLOCKS];
	size_t blocks_count;
}cache_set_t;

cache_set_t* init_cache_set() {
	cache_set_t* cache_set = malloc(sizeof(cache_set_t));

	if (!cache_set) {
		puts("Error initializing sets from cache");
		abort();
	}

	cache_set->blocks_count = BLOCKS;

	for (int i = 0; i < cache_set->blocks_count; ++i) {
		cache_set->blocks[i] = init_block();
	}

	return cache_set;
}

void destroy_cache_set(cache_set_t* cache_set) {
	if (cache_set) {
		for (int i = 0; i < cache_set->blocks_count; ++i) {
			destroy_block(cache_set->blocks[i]);
		}
	}
	cache_set->blocks_count = 0;
	free(cache_set);
}

/* ---------------------------------------------------- */

/* ---------------- CACHE ------------------ */

typedef struct cache {
	int size;
	cache_set_t* ways[WAYS];
	int misses;
	int hits;
} cache_t;

void init() {

	CACHE = malloc(sizeof(cache_t));
	if (!CACHE) {
		puts("Error initializing cache");
		abort();
	}

	CACHE->size = CACHE_SIZE;
	CACHE->hits = 0;
	CACHE->misses = 0;

	for (int i = 0; i < WAYS; ++i) {
		CACHE->ways[i] = init_cache_set();
	}
}

void destroy() {
	if (CACHE) {
		for (int i = 0; i < WAYS; ++i) {
			destroy_cache_set(CACHE->ways[i]);
		}
	}
	free(CACHE);
}

/* ---------------------------------------------------- */

unsigned int find_set(unsigned int address) {
	unsigned int tag = get_tag(address);
	unsigned int index = get_index(address);

	int set = -1;
	int i;

	for (i = 0; i < WAYS; ++i) {
		if ((CACHE->ways[i]->blocks[index]->valid == 1) &&
			 (CACHE->ways[i]->blocks[index]->tag == tag)
		) {
			set = i;
			break;
		}
	}

	return set;
}

unsigned int select_oldest(unsigned int setnum) {
	if (!CACHE) {
		puts("Cache is not initialized");
		return 0;
	}

	int oldest = 0;

	time_t timeA, timeB;
	double seconds;

	for (int i = 0; i < WAYS; ++i) {
		timeA = CACHE->ways[i]->blocks[setnum]->lastUpdate;
		timeB = CACHE->ways[oldest]->blocks[setnum]->lastUpdate;
		seconds = difftime(timeA, timeB);
		
		if (seconds < 0) {
			oldest = i;
		}
	}
	return oldest;
}

int is_dirty(int way, int setnum) {
	if (!CACHE) {
		puts("ERROR: The Cache isn't initialized");
		return 0;
	}
	
	if (WAYS < way) {
		puts("ERROR: The Cache Set especified don't exists");
		return 0;
	}else if(!CACHE->ways[way-1]) {
		puts("ERROR: The Cache Set especified isn't initialized");
		return 0;
	}

	if(CACHE->ways[way-1]->blocks_count < setnum) {
		puts("ERROR: The Block especified in the Cache Set don't exists");
		return 0;
	}else if(!CACHE->ways[way-1]->blocks[setnum-1]) {
		puts("ERROR: This Block especified in the Cache Set isn't initialized");
		return 0;
	}

	return CACHE->ways[way-1]->blocks[setnum-1]->dirty;
}

void read_tocache(unsigned int blocknum, unsigned int way, unsigned int set) {

  if (MAIN_MEMORY->blocks_count < blocknum) {
		puts("The main memory block especified does not exists");
		return;
	}
	else if (!MAIN_MEMORY->blocks[blocknum]) {
		puts("The Main Memory data especified is not initialized");
		return;
	}

	// Reading from main memory
  char* data_in_memory = malloc(BLOCK_SIZE * sizeof(char));
  strcpy(data_in_memory, MAIN_MEMORY->blocks[blocknum]->data);

	// Saving data to way & set
  strcpy(CACHE->ways[way]->blocks[set]->data, data_in_memory);
  CACHE->ways[way]->blocks[set]->dirty = 0;
  CACHE->ways[way]->blocks[set]->valid = 1;
  CACHE->ways[way]->blocks[set]->lastUpdate = time(NULL);
  CACHE->ways[way]->blocks[set]->tag = get_tag(blocknum);
}

unsigned char read_byte(unsigned int address) {
	unsigned int index = get_index(address);
	unsigned int offset = get_offset(address);

	char value;
	int set;

	// Looks in cache
	set = find_set(address);
	if (set != -1) {
		CACHE->hits++;
		value = *(CACHE->ways[set]->blocks[index]->data + offset);
	} 
	else {
		CACHE->misses++;
		unsigned int way_oldest_block = select_oldest(index);
		read_tocache(address >> SHIFT_OFFSET, way_oldest_block, index);
		set = find_set(address);
		if (set != -1) {
			value = *(CACHE->ways[set]->blocks[index]->data + offset);
		} 
		else {
			puts("Error reading block from main memory");
			abort();
		}
	}

	return (int) value;
}

void write_byte(unsigned int address, unsigned char value) {
	unsigned int tag = get_tag(address);
	unsigned int index = get_index(address);
	unsigned int offset = get_offset(address);

	// If block is in cache, then update it and then update main memory
	int set = find_set(address);
	if (set != -1) {
		CACHE->hits++;
		*(CACHE->ways[set]->blocks[index]->data + offset) = value;
		CACHE->ways[set]->blocks[index]->dirty = 0;
		CACHE->ways[set]->blocks[index]->valid = 1;
		CACHE->ways[set]->blocks[index]->lastUpdate = time(NULL);
		CACHE->ways[set]->blocks[index]->tag = tag;
	}
	else {
		CACHE->misses++;
	}

	// Update main memory
  int blocknum = address >> SHIFT_OFFSET;
  char* data = malloc(sizeof(char));
  (*data) = value;
  strcpy(MAIN_MEMORY->blocks[blocknum]->data, data);

  free(data);
	return;
}

float get_miss_rate() {
	if ((CACHE->misses + CACHE->hits) == 0) {
		return 0;
	}
	return (float) CACHE->misses / ( (float) CACHE->misses + (float) CACHE->hits ) * 100;
}
