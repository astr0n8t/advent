#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// Matter definitions
#define AIR 0
#define CUBE 1
#define WATER 2

// Cube struct to hold 3d coordinates
struct Cube {
	int x;
	int y;
	int z;
};

// 3d array List-like struct
struct Space {
	int*** array;
	int width;
	int height;
	int depth;
};

// List of cubes
struct CubeList {
	struct Cube* array;
	size_t size;
};

// We need a function definition since these functions call
// each other
void fillcracks(struct Space environment, struct Cube c);

// Function to fill a spot with water and continue to fill cracks
void fill(struct Space environment, int x, int y, int z) {
	struct Cube tmp = {.x=x, .y=y, .z=z};
	if (environment.array[x][y][z] == AIR) {
		environment.array[x][y][z] = WATER;
		fillcracks(environment, tmp);
	}
	return;
}


// Brute force recursive function to find all air pockets
// that aren't visible from the surface but are still exposed
void fillcracks(struct Space environment, struct Cube c) {
	// Check the x+1 case
	fill(environment, c.x+1, c.y, c.z);
	fill(environment, c.x-1, c.y, c.z);
	fill(environment, c.x, c.y+1, c.z);
	fill(environment, c.x, c.y-1, c.z);
	fill(environment, c.x, c.y, c.z+1);
	fill(environment, c.x, c.y, c.z-1);
	return;
}


// Brute force function to fill all outside air with water
void waterfill(struct Space environment) {
	struct Cube tmp;

	// Evaluate from front and back
	for (int x=0; x<environment.width; x++) {
		for (int y=0; y<environment.height; y++) {
			for (int z=0; z<environment.depth && environment.array[x][y][z] != CUBE; z++) {
				environment.array[x][y][z] = WATER;
			}
			for (int z=environment.depth-1; z>-1 && environment.array[x][y][z] != CUBE; z--) {
				environment.array[x][y][z] = WATER;
			}
		}
	}
	// Evaluate from left and right
	for (int y=0; y<environment.height; y++) {
		for (int z=0; z<environment.depth; z++) {
			for (int x=0; x<environment.width && environment.array[x][y][z] != CUBE; x++) {
				environment.array[x][y][z] = WATER;
			}
			for (int x=environment.width-1; x>-1 && environment.array[x][y][z] != CUBE; x--) {
				environment.array[x][y][z] = WATER;
			}
		}
	}
	// Evaulate from top and bottom
	for (int z=0; z<environment.depth; z++) {
		for (int x=0; x<environment.width; x++) {
			for (int y=0; y<environment.height && environment.array[x][y][z] != CUBE; y++) {
				environment.array[x][y][z] = WATER;
			}
			for (int y=environment.height-1; y>-1 && environment.array[x][y][z] != CUBE; y--) {
				environment.array[x][y][z] = 2;
			}
		}
	}

	// Find all cracks previously unfound
	for (int x=1; x<environment.width-1; x++) {
		for (int y=1; y<environment.height-1; y++) {
			for (int z=1; z<environment.depth-1; z++) {
				// If this is water we need to check if there is air around it
				// so call fillcracks on every water cube
				if (environment.array[x][y][z] == 2) {
					tmp.x = x;
					tmp.y = y;
					tmp.z = z;
					fillcracks(environment, tmp);
				}
			}
		}
	}
	return;
}

// Solve part1
int part1(struct Space environment, struct CubeList list) {
	// stack vars
	int result = 0;
	for (int i=0; i<list.size; i++) {
		// Quick calculation basically CUBEs are equal to 1 and AIR is equal to 0
		// water is not involved yet
		result = result + 6 - 
			environment.array[list.array[i].x+1][list.array[i].y][list.array[i].z] -
			environment.array[list.array[i].x-1][list.array[i].y][list.array[i].z] -
			environment.array[list.array[i].x][list.array[i].y+1][list.array[i].z] -
			environment.array[list.array[i].x][list.array[i].y-1][list.array[i].z] -
			environment.array[list.array[i].x][list.array[i].y][list.array[i].z+1] -
			environment.array[list.array[i].x][list.array[i].y][list.array[i].z-1];
	}
	return result;
}

// Solve part2
int part2(struct Space environment, struct CubeList list) {
	// Fill the environment exterior with water
	waterfill(environment);
	// Stack vars
	int result = 0;
	for (int i=0; i<list.size; i++) {
		// Check every block to the left, right, up, down, back, front and see if it is water
		if(environment.array[list.array[i].x+1][list.array[i].y][list.array[i].z]==WATER) {
			result++;
		}
		if(environment.array[list.array[i].x-1][list.array[i].y][list.array[i].z]==WATER) {
			result++;
		}
		if(environment.array[list.array[i].x][list.array[i].y+1][list.array[i].z]==WATER) {
			result++;
		}
		if(environment.array[list.array[i].x][list.array[i].y-1][list.array[i].z]==WATER) {
			result++;
		}
		if(environment.array[list.array[i].x][list.array[i].y][list.array[i].z+1]==WATER) {
			result++;
		}
		if(environment.array[list.array[i].x][list.array[i].y][list.array[i].z-1]==WATER) {
			result++;
		}
	}
	return result;
}

// Function to parse the input file and return
// a string representing the wind
struct Space processinput(FILE *in_file, struct CubeList* list) {
	// Stack vars
	struct Space output = {.height=0, .width=0, .depth=0};
	char buffer[100];
	int curr = 0;

	// Get the total number of cubers
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		// Check if the line is blank
		if (buffer[0] != '\n') {
			list->size++;
		}
	}
	// Go back to beginning of file
	rewind(in_file);

	// Allocate our list memory
	list->array = (struct Cube*)malloc(sizeof(struct Cube)*list->size);

	// Read in our input file
	while(fscanf(in_file, "%d,%d,%d\n", &list->array[curr].x, &list->array[curr].y, 
				&list->array[curr].z) == 3) {
		// Check for maxes
		if (list->array[curr].x > output.width) {
			output.width = list->array[curr].x;
		}
		if (list->array[curr].y > output.height) {
			output.height = list->array[curr].y;
		}
		if (list->array[curr].z > output.depth) {
			output.depth = list->array[curr].z;
		}
		curr++;
	}
	// We need to add 2 to each dimension so we can safely check dimension +1 and -1
	output.width = output.width + 2;
	output.height = output.height + 2;
	output.depth = output.depth + 2;

	// Allocate our 3d space array
	output.array = (int***)malloc(sizeof(int*)*output.width + 
			sizeof(int**)*output.width*output.height + 
			sizeof(int)*output.width*output.height*output.depth);

	// Do some pointer math to setup our arrays properly
	for (int i=0;i<output.width; i++) {
		output.array[i] = (int **)((int **)(output.array + output.width) + output.height*i);
		for (int j=0; j<output.height; j++) {
			output.array[i][j] = (int*)((int*)(output.array + output.width + output.width*output.height) + 
					i*output.height*output.depth + j*output.depth);
			// Initialize the array to 0
			for (int k=0; k<output.depth; k++) {
				//printf("Setting %d %d %d\n", i, j, k);
				output.array[i][j][k] = 0;
			}
		}
	}

	// Set all of our cube locations
	for (int i=0; i<list->size; i++) {
		output.array[list->array[i].x][list->array[i].y][list->array[i].z] = CUBE;
	}

	// Return the space
	return output;
}

int main(int argc, char *argv[])
{
	// Read in our input file
	FILE *in_file = fopen("input.txt", "r");
	if (in_file == NULL) {
		printf("Could not open input file!");
		exit(-1);
	}
	// Declare stack vars
	// Get our list and environment from the input file
	struct CubeList list = {.size=0};
	struct Space environment = processinput(in_file, &list);
	// Solve part 1
	printf("The total surface area is %d\n", part1(environment, list));
	// Solve part 2
	printf("The total exterior surface area is %d\n", part2(environment, list));

	// Clean up memory
	free(list.array);
	free(environment.array);
	// Close our file
	fclose(in_file);

	return EXIT_SUCCESS;
}
