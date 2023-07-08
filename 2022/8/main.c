#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Define each value as a prime so we can get whether a tree
// is visible from a certain direction by checking whether 
// it is divisible by that prime.  I didn't end up needing this
// for the challenge but I thought it was cool enough to leave
// in the code
#define VISIBLE_UP 2
#define VISIBLE_DOWN 3
#define VISIBLE_LEFT 5
#define VISIBLE_RIGHT 7

// Check if the tree is visible from the top down
void top(int** forest, int** visible, int* height, int max, int x, int y) {
	if (forest[x][y] > height[y]) {
		height[y] = forest[x][y];
		visible[x][y] = visible[x][y]*VISIBLE_UP;
	}
}
// Check if the tree is visible from left to right
void left(int** forest, int** visible, int* height, int max, int y, int x) {
	if (forest[x][y] > height[x+max]) {
		height[x+max] = forest[x][y];
		visible[x][y] = visible[x][y]*VISIBLE_LEFT;
	}
}
// Check if the tree is visible from right to left
void right(int** forest, int** visible, int* height, int max, int y, int x) {
	y = max - y - 1;
	if (forest[x][y] > height[x+max*2]) {
		height[x+max*2] = forest[x][y];
		visible[x][y] = visible[x][y]*VISIBLE_RIGHT;
	}
}
// Check if the tree is visible from bottom to top
void bottom(int** forest, int** visible, int* height, int max, int x, int y) {
	x = max - x - 1;
	if (forest[x][y] > height[y+max*3]) {
		height[y+max*3] = forest[x][y];
		visible[x][y] = visible[x][y]*VISIBLE_DOWN;
	}
}

// This function gets the number of trees we can see from a specific
// tree for part two;  I wanted to find a way to do this function
// a bit better, but for some reason this is the best I could 
// come up with
int score(int** forest, int** visible, int max, int x, int y) {
	int score = 1;
	int tmp;

	// Check to make sure we're not on an edge
	if (x && y && (max-x-1) && (max-y-1)) {
		tmp = 0;
		// Check from the top
		while (x-tmp-1 > -1) {
			tmp = tmp + 1;
			// Our stopping condition
			// but we need to note that we can still
			// see this tree so we can't include it
			// in the while loop
			if (forest[x][y] <= forest[x-tmp][y]) {
				break;
			}
		}
		score = score*tmp;
		tmp = 0;
		// Check from the left
		while (y-tmp-1 > -1) {
			tmp = tmp + 1;
			if (forest[x][y] <= forest[x][y-tmp]) {
				break;
			}
		}
		score = score*tmp;
		tmp = 0;
		// Check from the right
		while (y+tmp+1 < max) {
			tmp = tmp + 1;
			if (forest[x][y] <= forest[x][y+tmp]) {
				break;
			}
		}
		score = score*tmp;
		tmp = 0;
		// Check from the bottom
		while (x+tmp+1 < max) {
			tmp = tmp + 1;
			if (forest[x][y] <= forest[x+tmp][y]) {
				break;
			}
		}
		score = score*tmp;
	}
	return score;
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
	int curr_score = 0;
	int highest_score = 0;

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

	// Go through our arrays and find all the trees which are visible
	for (int i=0;i<size_of_forest;i=i+1) {
		for (int j=0;j<size_of_forest;j=j+1) {
			top(forest, visible, height, size_of_forest, i, j);
			left(forest, visible, height, size_of_forest, i, j);
			right(forest, visible, height, size_of_forest, i, j);
			bottom(forest, visible, height, size_of_forest, i, j);
		}
	}
	// Go through the arrays and add up the number of visible trees
	// Also find the tree with the most trees visible from it
	for (int i=0;i<size_of_forest;i=i+1) {
		for (int j=0;j<size_of_forest;j=j+1) {
			// Check if tree is visible from outside the forest
			if (visible[i][j] != 1) {
				num_visible = num_visible + 1;
			}
			// Check if this tree can see more trees than the previous best
			curr_score = score(forest, visible, size_of_forest, i, j);
			if (curr_score > highest_score) {
				highest_score = curr_score;
			}
		}
	}

	// Solve part one
	printf("Number of trees visible: %d\n", num_visible);
	// Solve part two
	printf("Best tree score: %d\n", highest_score);

	// Free all the memory we allocated
	free(forest);
	free(visible);
	free(height);

	fclose(in_file);
	return EXIT_SUCCESS;
}
