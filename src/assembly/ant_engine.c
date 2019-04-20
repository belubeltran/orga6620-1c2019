#include "ant_engine.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

extern void advanceAnt(ant_t *, __uint32_t, __uint32_t);
extern orientation_t getNextOrientation(orientation_t , rotation_t );

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

