#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "main_memory.h"
#include "cache.h"

#define NONE 0
#define READ 1
#define WRITE 2
#define MISS_RATE 3
#define FLUSH 4

const int MAX_LENGTH = 256;
const int MAX_LENGTH_LINES = 256;
const int MAX_ADDRESS = 64 * 1024;
const int SIZE_OF_BYTE = 256;

main_memory_t* MAIN_MEMORY;
cache_t* CACHE;

bool validate_adress(int address) {
	if (address >= MAX_ADDRESS){
		printf("The specified address is too large. The address limit is: %d\n", MAX_ADDRESS);
		return false;
	}
	return true;
}

bool validate_value(int value) {
	if (value >= SIZE_OF_BYTE){
		printf("The specified value is bigger that a byte\n");
		return false;
	}
	return true;
}

bool process(char* line) {
	char *token = strtok(line, " ");
	int operation = NONE;
	int register_1, register_2;

  if(token) {
		if (strcmp(token, "MR\n") == 0) {
			operation = MISS_RATE;
		}
		else if (strcmp(token, "FLUSH\n") == 0) {
			operation = FLUSH;
		}
		else if (strcmp(token, "R") == 0) {
			operation = READ;
			token = strtok(NULL, " ");
			if (!token) {
				printf("Error in read command: incorrect amount of parameters\n");
				return false;
			}
			register_1 = atoi(token);
		}
		else if (strcmp(token, "W") == 0){
			operation = WRITE;
			token = strtok(NULL, ",");
			if (!token) {
				printf("Error in write command: incorrect amount of parameters\n");
				return false;
			}
			register_1 = atoi(token);
    	token = strtok(NULL, " ");
    	if (!token) {
				printf("Error in write command: incorrect amount of parameters\n");
				return false;
			}
			register_2 = atoi(token);
		}
		else {
			printf("Error reading next line: command not found\n");
			return false;
		}
  }

  int result;
  switch(operation) {
  	case READ:
			if (!validate_adress(register_1)) {
				return false;
			}
			result = read_byte(register_1);
			printf("%d\n", result >= 0 ? result:result+SIZE_OF_BYTE);
  		break;
  	case WRITE:
  		if (!validate_adress(register_1) || !validate_value(register_2)) {
  			return false;
  		}
  		write_byte(register_1, register_2);
  		break;
  	case MISS_RATE:
			printf("%.2f\n", get_miss_rate());
  		break;
  	case FLUSH:
			destroy();
			init();
  		break;
  }

  return true;
}

void read_file(char* filename) {
	char chars[MAX_LENGTH];
	FILE *file = fopen(filename,"r");

	if (file == NULL){
		printf("No such file\n");
		exit(1);
	} else {
		fgets(chars,1024,file);
		while (feof(file) == 0) {			
			if (!process(chars)) break;
			fgets(chars,1024,file);
		}
	}

	fclose(file);
}

int main(int argc, char *argv[]) {
	init_main_memory();
	init();

	read_file(argv[argc-1]);

	destroy_main_memory();
	destroy();
	return 0;
}
