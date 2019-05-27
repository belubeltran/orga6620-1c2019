#ifndef CACHE_H
#define CACHE_H

typedef struct cache cache_t;

extern cache_t* CACHE;

/* Initializes the cache blocks as invalid and the miss rate
	to 0. */
void init();

/* Returns the offset given in address */
unsigned int get_offset(unsigned int address);

/* Returns the cache set that to which the address maps. */
unsigned int find_set(unsigned int address);

/* Returns the way in which the oldest block is */
unsigned int select_oldest(unsigned int setnum);

/* Reads the block blocknum from memory and stores it in the
 	corresponding way and set in the cache. */
void read_tocache(unsigned int blocknum, unsigned int way, unsigned int set);

/* Returns the value corresponding to the address position. */
unsigned char read_byte(unsigned int address);

/* Writes the value value to the correct position of
 	the block corresponding to the address. */
void write_byte(unsigned int address, unsigned char value);

/* Returns the percentage of misses since the cache was
	initialized. */
float get_miss_rate();

/* Destroys the Cache */
void destroy();

#endif // CACHE_H
