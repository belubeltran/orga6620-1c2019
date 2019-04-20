#ifndef __ARTIST_ANT_H__
#define __ARTIST_ANT_H__

#include <unistd.h>

void* paint(void *ant, void *grid, void *palette, void *rules,  __uint32_t iterations);

void* make_rules(char *spec);

void* make_palette(char *colours);

void* make_grid(__uint32_t w, __uint32_t h, colour_t c);

void* make_ant(__uint32_t xini, __uint32_t yini);

void grid_out();

__uint32_t as_int(void *arg, __uint32_t from, __uint32_t to);

#endif /* __ARTIST_ANT_H__ */

