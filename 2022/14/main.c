#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SANDREST 1
#define SANDFALL 0

#define AIR '.'
#define ROCK '#'
#define SAND 'o'

// struct to hold points
struct Point {
	int x;
	int y;
};

// struct to hold the cave
struct Grid {
	char** array;
	struct Point start;
	struct Point end;
	struct Point dimensions;
};

// Helper functions
int min(int a, int b) {
	return a < b ? a : b;
}
int max(int a, int b) {
	return a > b ? a : b;
}

// This funciton parses our input file and gives us an equivalent Grid struct
struct Grid parseinput(FILE* in_file) {
	// Declare some stack vars
	char buffer[500];
	struct Grid cave = {.start = {.x = -1, .y = 0}, .end = {.x = -1, .y = -1}};
	struct Point curr;
	struct Point prev;

	// Iterate over the file to get the dimensions of the cave
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		char* token = strtok(buffer, " -> ");
		while (token != NULL) {
			prev = curr;
			if (sscanf(token, "%d,%d", &curr.x, &curr.y) == 2) {
				if (curr.x > cave.end.x || cave.end.x == -1) {
					cave.end.x = curr.x;
				}
				if (curr.y > cave.end.y || cave.end.y == -1) {
					cave.end.y = curr.y;
				}
				if (curr.x < cave.start.x || cave.start.x == -1) {
					cave.start.x = curr.x;
				}
			}
			token = strtok(NULL, " -> ");
		}
	}
	rewind(in_file);
	// Compute cave dimensions
	cave.dimensions.x = cave.end.x - cave.start.x + 1;
	cave.dimensions.y = cave.end.y + 1;

	// Allocate our cave array and set every value to .
	cave.array = (char**)malloc(cave.dimensions.y*sizeof(char*)+(cave.dimensions.x+1)*cave.dimensions.y*sizeof(char));
	for (int i=0;i<cave.dimensions.y;i++) {
		cave.array[i] = (char *)((char *)(cave.array+cave.dimensions.y)+(cave.dimensions.x+1)*i);
		for (int j=0;j<cave.dimensions.x; j++) {
			cave.array[i][j] = AIR;
		}
	}
	// Iterate over the file again and set our rock formations
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		char* token = strtok(buffer, " -> ");
		sscanf(token, "%d,%d", &curr.x, &curr.y);
			if (curr.x==6) {
				printf("%s\n", token);
			}
		while ((token = strtok(NULL, " -> ")) &&  token != NULL) {
			prev = curr;
			if (sscanf(token, "%d,%d", &curr.x, &curr.y) == 2) {
				if (prev.x == curr.x) {
					for (int i = min(prev.y, curr.y); i<max(prev.y, curr.y)+1; i++) {
						cave.array[i][curr.x-cave.start.x] = ROCK;
					}
				}
				else {
					for (int i = min(prev.x, curr.x); i<max(prev.x,curr.x)+1; i++) {
						cave.array[curr.y][i-cave.start.x] = ROCK;
					}
				}
			}
		}
	}
	// Return the processed cave
	return cave;
}

// This function generates a single piece of sand
// it returns SANDFALL if the piece fell into the abyss
// or SANDREST if the piece came to rest
int sandgen(struct Grid cave) {
	int result = SANDFALL;
	struct Point curr = {.x=500, .y=0};

	while (curr.x <= cave.end.x && curr.y <= cave.end.y && 
			curr.x >= cave.start.x && curr.y >= cave.start.y &&
			result == SANDFALL) {
		if (curr.y == cave.end.y || cave.array[curr.y+1][curr.x-cave.start.x] == AIR) {
			curr.y++;
		}
		else if (curr.x == cave.start.x || cave.array[curr.y+1][curr.x-1-cave.start.x] == AIR) {
			curr.y++;
			curr.x--;
		}
		else if (curr.x == cave.end.x || cave.array[curr.y+1][curr.x+1-cave.start.x] == AIR) {
			curr.y++;
			curr.x++;
		}
		else {
			result = SANDREST;
			cave.array[curr.y][curr.x-cave.start.x] = SAND;
		}
	}

	return result;
}

int main(int argc, char *argv[])
{
	// Read in our input file
	FILE *in_file = fopen("input.txt", "r");
	if (in_file == NULL) {
		printf("Could not open input file!");
		exit(-1);
	}
	// Declare our stack vars
	struct Grid cave = parseinput(in_file);
	int num_rounds = 0;

	while(sandgen(cave) != SANDFALL) {
		num_rounds++;
	}
	for (int y=0; y<cave.dimensions.y; y++) {
		printf("%d ", y);
		printf("%s\n", cave.array[y]);
	}


	printf("\nNum rounds %d\n", num_rounds);

	free(cave.array);

	fclose(in_file);
	return EXIT_SUCCESS;
}
