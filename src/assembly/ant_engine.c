#include "ant_engine.h"

#include <assert.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/* This file should be removed once all .S files are done */
extern getNextOrientation(orientation_t, rotation_t);
// orientation_t
// getNextOrientation(orientation_t current, rotation_t rotation)
// {
// 	switch(current) {
// 		case NORTH:
// 			if (rotation == RR) {
// 				return EAST;
// 			}
// 			return WEST;
// 		case EAST:
// 			if (rotation == RR) {
// 				return SOUTH;
// 			}
// 			return NORTH;
// 		case SOUTH:
// 			if (rotation == RR) {
// 				return WEST;
// 			}
// 			return EAST;
// 		case WEST:
// 			if (rotation == RR) {
// 				return NORTH;
// 			}
// 			return SOUTH;
// 	}
// }

void
advanceAnt(ant_t *ant, __uint32_t width, __uint32_t height)
{
	switch(ant->o) {
		case NORTH:
			if (ant->y > 0) {
				ant->y--;
			}
			else {
				ant->y = height - 1;
			}
			break;
		case EAST:
			if (ant->x < width) {
				ant->x = 0;
			}
			else {
				ant->x++;
			}
			break;
		case SOUTH:
			if (ant->y < height) {
				ant->y = 0;
			}
			else {
				ant->y++;
			}
			break;
		case WEST:
			if (ant->x > 0) {
				ant->x--;
			}
			else {
				ant->x = width - 1;
			}
			break;
	}
}

/* 
 * Tam grilla_t = 16
 * Tam ant_t = 12
*/
void*
paint(void *ant, void *grid, void *palette, void *rules,  __uint32_t iterations)
{
	char *p = (char*)palette + 1;
	char *r = (char*)rules;
	square_grid_t *g = (square_grid_t*)grid;
	ant_t *a = (ant_t *)ant;
	int i;

	// Assuming iterations <= # rules
	for (i = 0; i < iterations; i++) {
		g->grid[a->y][a->x] = *p;
		a->o = getNextOrientation(a->o, r[i]);
		advanceAnt(a, g->width, g->height);
		p++;
	}

  return grid;
}

void*
make_rules(char *spec)
{
  static char * index = "LR";
  int i;
  int pos = 0;
  int len = strlen(spec);
  char * res = (char *) malloc((char)floor(strlen(spec)/2));

  for (i = 0; i < len; i++) {
	  char *p = strchr(index, spec[i]);

	  if (p != NULL) {
	  	res[pos] = (char)(p - index);
	  	pos++;
	  }
  }

  return ((void*)res);
}

void*
make_palette(char *colours)
{
  static char * index = "RGBYNW";
  int i;
  int pos = 0;
  int len = strlen(colours);
  char * res = (char *) malloc((char)floor(strlen(colours)/2));

  for (i = 0; i < len; i++) {
	  char *p = strchr(index, colours[i]);

	  if (p != NULL) {
	  	res[pos] = (char)(p - index);
	  	pos++;
	  }
  }

  return ((void*)res);
}

