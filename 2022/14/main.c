#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Define constants
#define SANDREST 1
#define SANDFALL 0
#define SANDTOP 2

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

	// Allocate our cave array and set every value to air
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

// Deep copies a Grid struct
struct Grid deepcopy(struct Grid src) {
	// Make a stack copy of src
	struct Grid dst = src;
	// Allocate a new slice of memory for the array
	dst.array = (char**)malloc(dst.dimensions.y*sizeof(char*)+(dst.dimensions.x+1)*dst.dimensions.y*sizeof(char));
	// Iterate over our subarrays
	for (int i=0;i<dst.dimensions.y;i++) {
		// Fix our secondary pointers
		dst.array[i] = (char *)((char *)(dst.array+dst.dimensions.y)+(dst.dimensions.x+1)*i);
		// Copy the memory from the src to the dst
		memcpy(dst.array[i], src.array[i], sizeof(char)*(src.dimensions.x+1));
	}
	// Return the new Grid
	return dst;
}

// Adds an infinite cave floor to a grid at the specified height
// Destroys the old Grid
struct Grid addfloor(struct Grid src, int height) {
	// Make a copy of cave
	struct Grid dst = src;
	// Set the new dimensions
	dst.dimensions.y = height + 1;
	dst.end.y = height;
	// Re-allocate our memory
	dst.array = (char**)malloc(dst.dimensions.y*sizeof(char*)+(dst.dimensions.x+1)*dst.dimensions.y*sizeof(char));
	// Do pointer math and copy arrays for old subarrays
	for (int i=0;i<src.dimensions.y;i++) {
		dst.array[i] = (char *)((char *)(dst.array+dst.dimensions.y)+(dst.dimensions.x+1)*i);
		memcpy(dst.array[i], src.array[i], sizeof(char)*(src.dimensions.x+1));
	}
	// Do pointer math and fill the rest with air
	for (int i=src.dimensions.y; i<dst.dimensions.y; i++) {
		dst.array[i] = (char *)((char *)(dst.array+dst.dimensions.y)+(dst.dimensions.x+1)*i);
		for (int j=0; j<dst.dimensions.x; j++) {
			dst.array[i][j] = AIR;
		}
	}

	// Add the rock floor
	for (int i=0; i<dst.dimensions.x; i++) {
		dst.array[height][i] = ROCK;
	}

	// Free the original cave's memory
	free(src.array);

	// Return the new Grid
	return dst;
}

// Widens a Grid struct by 200 on either side
// 
struct Grid widen(struct Grid src) {
	struct Grid dst = src;
	// Increase the dimensions
	// It doesn't matter if start.x goes below zero
	// as we already calculate our index using this as an offset
	// so subtracting 10 from -100 will yield 110 in the array
	// which is correct
	// The instructions say to go to -infinity so this matches
	dst.start.x = dst.start.x - 100;
	dst.end.x = dst.end.x + 100;
	// We always increase the size of the array
	dst.dimensions.x = dst.dimensions.x + 200;
	// Re-allocate our memory
	dst.array = (char**)malloc(dst.dimensions.y*sizeof(char*)+(dst.dimensions.x+1)*dst.dimensions.y*sizeof(char));
	// Iterate over our subarrays
	for (int i=0;i<dst.dimensions.y;i++) {
		// Perform pointer math to setup subarray pointers
		dst.array[i] = (char *)((char *)(dst.array+dst.dimensions.y)+(dst.dimensions.x+1)*i);
		// Copy our old array shifted in by 100
		memcpy((char*)(dst.array[i]+100), src.array[i], sizeof(char)*src.dimensions.x);
		// Fill the first 100 spaces with air
		for (int j=0; j<100; j++) {
			dst.array[i][j] = AIR;
		}
		// Fill the last 100 spaces with air
		for (int j=dst.dimensions.x-100; j<dst.dimensions.x; j++) {
			dst.array[i][j] = AIR;
		}
	}

	// Free the original cave's memory
	free(src.array);

	// Return the new Grid
	return dst;
}


// This function generates a single piece of sand
// it returns SANDFALL if the piece fell into the abyss
// or SANDREST if the piece came to rest
int sandgen(struct Grid cave) {
	// Create stack vars
	int result = SANDFALL;
	struct Point curr = {.x=500, .y=0};

	// Iterate until the sand falls into the abyss or
	// the sand has come to rest
	while (curr.x <= cave.end.x && curr.y <= cave.end.y && 
			curr.x >= cave.start.x && curr.y >= cave.start.y &&
			result == SANDFALL) {

		// Check if the block below us is air
		if (curr.y == cave.end.y || cave.array[curr.y+1][curr.x-cave.start.x] == AIR) {
			// If it is move down
			curr.y++;
		}
		// Check if the block down and to the left is air
		else if (curr.x == cave.start.x || cave.array[curr.y+1][curr.x-1-cave.start.x] == AIR) {
			// If it is move down and to the left
			curr.y++;
			curr.x--;
		}
		// Check if the block down and to the right is air
		else if (curr.x == cave.end.x || cave.array[curr.y+1][curr.x+1-cave.start.x] == AIR) {
			// If it is move down and to the right
			curr.y++;
			curr.x++;
		}
		// Otherwise we've come to rest
		else {
			// Set our result to rest
			result = SANDREST;
			// Set the array position to sand
			cave.array[curr.y][curr.x-cave.start.x] = SAND;
			// Check if this is the topmost piece of sand
			if (curr.y == 0 && curr.x == 500) {
				// Set our result to indicate we've placed the last piece of sand
				result = SANDTOP;
			}
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
	struct Grid cave_floor = deepcopy(cave);
	// Add the floor to our second cave
	cave_floor = addfloor(cave_floor, cave_floor.end.y+2);
	int floor_result = SANDFALL;
	int num_rounds_void = 0;
	int num_rounds_floor = 0;

	// Part 1: Iterate until a piece of sand goes into the abyss
	while(sandgen(cave) != SANDFALL) {
		num_rounds_void++;
	}

	// Iterate until we've placed the last piece of sand
	while(floor_result != SANDTOP) {
		// Generate a new sand piece
		floor_result = sandgen(cave_floor);
		// Check if it fell into the abyss
		if (floor_result == SANDFALL) {
			// If it did, we need to widen the cave
			cave_floor = widen(cave_floor);
			// And then re-add our floor to stretch the length
			// of the cave
			cave_floor = addfloor(cave_floor, cave_floor.end.y);
		}
		else {
			// Otherwise the sand was placed
			num_rounds_floor++;
		}
	}

	// Solve part 1
	printf("Num rounds with void %d\n", num_rounds_void);
	// Solve part 2
	printf("Num rounds with floor %d\n", num_rounds_floor);

	// Free our memory
	free(cave.array);
	free(cave_floor.array);

	// Close our file
	fclose(in_file);
	return EXIT_SUCCESS;
}
