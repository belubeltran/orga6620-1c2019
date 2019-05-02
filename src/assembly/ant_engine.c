#include "ant_engine.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

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

