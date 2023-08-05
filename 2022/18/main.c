#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Basic Point struct
struct Cube {
	int x;
	int y;
	int z;
};

struct Space {
	int*** array;
	int width;
	int height;
	int depth;
};

struct CubeList {
	struct Cube* array;
	size_t size;
};

// Solve part1
int part1(struct Space environment, struct CubeList list) {
	int result = 0;
	for (int i=0; i<list.size; i++) {
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
int part2() {
	return 0;
}

// Function to parse the input file and return
// a string representing the wind
struct Space processinput(FILE *in_file, struct CubeList* list) {
	struct Space output = {.height=0, .width=0, .depth=0};
	char buffer[100];
	int curr = 0;

	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		// Check if the line is blank
		if (buffer[0] != '\n') {
			list->size++;
		}
	}
	rewind(in_file);

	list->array = (struct Cube*)malloc(sizeof(struct Cube)*list->size);

	// Read in our input file
	while(fscanf(in_file, "%d,%d,%d\n", &list->array[curr].x, &list->array[curr].y, 
				&list->array[curr].z) == 3) {
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
	output.width = output.width + 2;
	output.height = output.height + 2;
	output.depth = output.depth + 2;

	// Allocate our space array
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

	printf("Space dimensions: %d %d %d\n", output.width, output.height, output.depth);
	// Set all of our cube locations
	for (int i=0; i<list->size; i++) {
		//printf("Setting %d %d %d\n", list->array[i].x, list->array[i].y, list->array[i].z);
		output.array[list->array[i].x][list->array[i].y][list->array[i].z] = 1;
	}

	// Return the wind array
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
	// Get our wind from the input file
	struct CubeList list = {.size=0};
	struct Space environment = processinput(in_file, &list);
	// Solve part 1
	printf("The total exposed surface area is %d\n", part1(environment, list));

	// Clean up memory
	free(list.array);
	free(environment.array);
	// Close our file
	fclose(in_file);

	return EXIT_SUCCESS;
}
