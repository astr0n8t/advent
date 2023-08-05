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

void fillcracks(struct Space environment, struct Cube c) {
	struct Cube tmp = c;
	if (environment.array[c.x+1][c.y][c.z] == 0) {
		tmp.x = c.x+1;
		environment.array[c.x+1][c.y][c.z] = 2;
		fillcracks(environment, tmp);
	}
	if (environment.array[c.x-1][c.y][c.z] == 0) {
		tmp = c;
		tmp.x = c.x-1;
		environment.array[c.x-1][c.y][c.z] = 2;
		fillcracks(environment, tmp);
	}
	if (environment.array[c.x][c.y+1][c.z] == 0) {
		tmp = c;
		tmp.y = c.y+1;
		environment.array[c.x][c.y+1][c.z] = 2;
		fillcracks(environment, tmp);
	}
	if (environment.array[c.x][c.y-1][c.z] == 0) {
		tmp = c;
		tmp.y = c.y-1;
		environment.array[c.x][c.y-1][c.z] = 2;
		fillcracks(environment, tmp);
	}
	if (environment.array[c.x][c.y][c.z+1] == 0) {
		tmp = c;
		tmp.z = c.z+1;
		environment.array[c.x][c.y][c.z+1] = 2;
		fillcracks(environment, tmp);
	}
	if (environment.array[c.x][c.y][c.z-1] == 0) {
		tmp = c;
		tmp.z = c.z-1;
		environment.array[c.x][c.y][c.z-1] = 2;
		fillcracks(environment, tmp);
	}
	return;
}


void waterfill(struct Space environment) {
	struct Cube tmp;

	for (int x=0; x<environment.width; x++) {
		for (int y=0; y<environment.height; y++) {
			for (int z=0; z<environment.depth; z++) {
				if (environment.array[x][y][z]==1) {
					break;
				}
				else {
					environment.array[x][y][z] = 2;
				}
			}
			for (int z=environment.depth-1; z>-1; z--) {
				if (environment.array[x][y][z]==1) {
					break;
				}
				else {
					environment.array[x][y][z] = 2;
				}
			}
		}
	}
	for (int y=0; y<environment.height; y++) {
		for (int z=0; z<environment.depth; z++) {
			for (int x=0; x<environment.width; x++) {
				if (environment.array[x][y][z]==1) {
					break;
				}
				else {
					environment.array[x][y][z] = 2;
				}
			}
			for (int x=environment.width-1; x>-1; x--) {
				if (environment.array[x][y][z]==1) {
					break;
				}
				else {
					environment.array[x][y][z] = 2;
				}
			}
		}
	}
	for (int z=0; z<environment.depth; z++) {
		for (int x=0; x<environment.width; x++) {
			for (int y=0; y<environment.height; y++) {
				if (environment.array[x][y][z]==1) {
					break;
				}
				else {
					environment.array[x][y][z] = 2;
				}
			}
			for (int y=environment.height-1; y>-1; y--) {
				if (environment.array[x][y][z]==1) {
					break;
				}
				else {
					environment.array[x][y][z] = 2;
				}
			}
		}
	}

	for (int x=1; x<environment.width-1; x++) {
		for (int y=1; y<environment.height-1; y++) {
			for (int z=1; z<environment.depth-1; z++) {
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
int part2(struct Space environment, struct CubeList list) {
	waterfill(environment);
	int result = 0;
	int collision_check = 0;
	for (int i=0; i<list.size; i++) {
		if(environment.array[list.array[i].x+1][list.array[i].y][list.array[i].z]==2) {
			result++;
		}
		if(environment.array[list.array[i].x-1][list.array[i].y][list.array[i].z]==2) {
			result++;
		}
		if(environment.array[list.array[i].x][list.array[i].y+1][list.array[i].z]==2) {
			result++;
		}
		if(environment.array[list.array[i].x][list.array[i].y-1][list.array[i].z]==2) {
			result++;
		}
		if(environment.array[list.array[i].x][list.array[i].y][list.array[i].z+1]==2) {
			result++;
		}
		if(environment.array[list.array[i].x][list.array[i].y][list.array[i].z-1]==2) {
			result++;
		}
	}
	return result;
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
