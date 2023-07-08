#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define VISIBLE_UP 2
#define VISIBLE_DOWN 3
#define VISIBLE_LEFT 5
#define VISIBLE_RIGHT 7

void up(int** forest, int** visible, int* height, int max, int x, int y) {
	if (forest[x][y] > height[y]) {
		height[y] = forest[x][y];
		visible[x][y] = visible[x][y]*VISIBLE_UP;
	}
}
void left(int** forest, int** visible, int* height, int max, int y, int x) {
	if (forest[x][y] > height[x+max]) {
		height[x+max] = forest[x][y];
		visible[x][y] = visible[x][y]*VISIBLE_LEFT;
	}
}
void right(int** forest, int** visible, int* height, int max, int y, int x) {
	y = max - y - 1;
	if (forest[x][y] > height[x+max*2]) {
		height[x+max*2] = forest[x][y];
		visible[x][y] = visible[x][y]*VISIBLE_RIGHT;
	}
}
void down(int** forest, int** visible, int* height, int max, int x, int y) {
	x = max - x - 1;
	if (forest[x][y] > height[y+max*3]) {
		height[y+max*3] = forest[x][y];
		visible[x][y] = visible[x][y]*VISIBLE_DOWN;
	}
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
	// and root fs
	int** forest;
	int** visible;
	int* height;
	char curr;
	int size_of_forest = 0;
	int num_visible = 0;

	// Iterate to get length of line
	while(curr != EOF && curr != '\n') {
		curr = getc(in_file);
		if (curr != '\n') {
			size_of_forest = size_of_forest + 1;
		}
	}
	// Go back to beginning of file
	rewind(in_file);
	// Allocate our arrays
	forest = (int**)malloc(sizeof(int*)*size_of_forest+sizeof(int)*size_of_forest*size_of_forest);
	visible = (int**)malloc(sizeof(int*)*size_of_forest+sizeof(int)*size_of_forest*size_of_forest);
	height = (int*)malloc(sizeof(int)*size_of_forest*4);
	// Do some pointer math to setup our arrays properly
	for (int i=0;i<size_of_forest;i=i+1) {
		forest[i] = (int *)((int *)(forest+size_of_forest)+size_of_forest*i);
		visible[i] = (int *)((int *)(visible+size_of_forest)+size_of_forest*i);
		height[i] = -1;
		height[i+size_of_forest] = -1;
		height[i+size_of_forest*2] = -1;
		height[i+size_of_forest*3] = -1;
	}

	// Go through the file and grab each tree
	for (int i=0;i<size_of_forest;i=i+1) {
		for (int j=0;j<size_of_forest;j=j+1) {
			curr = getc(in_file);
			// Get the height of the tree at this location
			forest[i][j] = curr - '0';
			visible[i][j] = 1;
		}
		// Get rid of newline
		getc(in_file);
	}

	// print out visible
	// Iterate for all except the last line
	for (int i=0;i<size_of_forest;i=i+1) {
		for (int j=0;j<size_of_forest;j=j+1) {
			up(forest, visible, height, size_of_forest, i, j);
			left(forest, visible, height, size_of_forest, i, j);
			right(forest, visible, height, size_of_forest, i, j);
			down(forest, visible, height, size_of_forest, i, j);
		}
	}
	for (int i=0;i<size_of_forest;i=i+1) {
		for (int j=0;j<size_of_forest;j=j+1) {
			if (visible[i][j] != 1) {
				num_visible = num_visible + 1;
			}
		}
	}

	for (int i=0;i<size_of_forest;i=i+1) {
		for (int j=0;j<size_of_forest;j=j+1) {
			printf("%4d", forest[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	for (int i=0;i<size_of_forest;i=i+1) {
		for (int j=0;j<size_of_forest;j=j+1) {
			printf("%4d", visible[i][j]);
		}
		printf("\n");
	}

	// Solve part one
	printf("Number of trees visible: %d\n", num_visible);

	// Free all the memory we allocated
	free(forest);
	free(visible);
	free(height);

	fclose(in_file);
	return EXIT_SUCCESS;
}
