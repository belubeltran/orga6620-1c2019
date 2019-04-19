#include "ant_engine.h"

#include <assert.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


void*
paint(void *ant, void *grid, void *palette, void *rules,  __uint32_t iterations)
{
  panic ("Implement me!");
  return grid;
}

void*
make_rules(char *spec)
{
  static char * index = "LR";
  int i;
  int len = strlen(spec);
  int * res = (int *) malloc((int)floor(strlen(spec)/2));

  for (i = 0; i < len; i++) {
	  char *p = strchr(index, spec[i]);

	  if (p != NULL) {
	  	res[i] = p - index;
	  }
  }

  return ((void*)res);
}

void*
make_palette(char *colours)
{
  static char * index = "RGBYNW";
  int i;
  int len = strlen(colours);
  int * res = (int *) malloc((int)floor(strlen(colours)/2));

  for (i = 0; i < len; i++) {
	  char *p = strchr(index, colours[i]);

	  if (p != NULL) {
	  	res[i] = p - index;
	  }
  }

  return ((void*)res);
}

