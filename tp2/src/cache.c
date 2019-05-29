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

unsigned int get_tag(unsigned int address) {
	return (address & AND_TAG) >> SHIFT_TAG;
}

unsigned int get_index(unsigned int address) {
	return (address & AND_INDEX) >> SHIFT_INDEX;
}

unsigned int get_offset(unsigned int address) {
	return (address & AND_OFFSET);
}

/* ----------------- BLOCK ------------------- */

typedef struct block{
	time_t lastUpdate;
	int valid;
	int tag;
	char* data;
}block_t;

block_t* init_block() {
	block_t* block = malloc(sizeof(block_t));

	if (!block) {
		puts("Error initializing blocks from cache");
		abort();
	}
	
	block->data = malloc(BLOCK_SIZE * sizeof(char));

	if (!block->data) {
		puts("Error initializing data blocks from cache");
		abort();
	}
	
	block->lastUpdate = time(NULL);
	block->valid = 0;

	return block;
}

void destroy_block(block_t* block) {
	if(block) {
		free(block->data);
	}
	free(block);
}

/* ---------------------------------------------------- */

/* ----------------- SET ------------------- */

typedef struct cache_set{
	block_t* blocks[BLOCKS];
}cache_set_t;

cache_set_t* init_cache_set() {
	cache_set_t* cache_set = malloc(sizeof(cache_set_t));

	if (!cache_set) {
		puts("Error initializing sets from cache");
		abort();
	}

	for (int i = 0; i < BLOCKS; ++i) {
		cache_set->blocks[i] = init_block();
	}

	return cache_set;
}

void destroy_cache_set(cache_set_t* cache_set) {
	if (cache_set) {
		for (int i = 0; i < BLOCKS; ++i) {
			destroy_block(cache_set->blocks[i]);
		}
	}

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

void read_tocache(unsigned int blocknum, unsigned int way, unsigned int set) {
  if (MAIN_MEMORY->blocks_count < blocknum) {
		puts("The main memory block especified does not exists");
		return;
	}
	else if (!MAIN_MEMORY->blocks[blocknum]) {
		puts("The Main Memory data especified is not initialized");
		return;
	}

	// Time
	sleep(1);

	// Reading from main memory
  char* data_in_memory = malloc(BLOCK_SIZE * sizeof(char));
  strcpy(data_in_memory, MAIN_MEMORY->blocks[blocknum]->data);

	// Saving data to way & set
  strcpy(CACHE->ways[way]->blocks[set]->data, data_in_memory);
  CACHE->ways[way]->blocks[set]->valid = 1;
  CACHE->ways[way]->blocks[set]->lastUpdate = time(NULL);
  CACHE->ways[way]->blocks[set]->tag = get_tag(blocknum << SHIFT_OFFSET);

  free(data_in_memory);
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

	// Time
	sleep(1);

	// If block is in cache, then update it and then update main memory
	int set = find_set(address);
	if (set != -1) {
		CACHE->hits++;
		*(CACHE->ways[set]->blocks[index]->data + offset) = value;
		CACHE->ways[set]->blocks[index]->valid = 1;
		CACHE->ways[set]->blocks[index]->lastUpdate = time(NULL);
		CACHE->ways[set]->blocks[index]->tag = tag;
	}
	else {
		CACHE->misses++;
	}

	// Update main memory
  int blocknum = address >> SHIFT_OFFSET;
  MAIN_MEMORY->blocks[blocknum]->data[offset] = value;

	return;
}

float get_miss_rate() {
	if ((CACHE->misses + CACHE->hits) == 0) {
		return 0;
	}
	return (float) CACHE->misses / ( (float) CACHE->misses + (float) CACHE->hits ) * 100;
}
