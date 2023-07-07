#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define VISIBLE_UP 2
#define VISIBLE_DOWN 3
#define VISIBLE_LEFT 5
#define VISIBLE_RIGHT 7

void up(int** forest, int** visible, int size_of_forest) {

	// Iterate for all except the last line
	for (int i=1;i<size_of_forest-1;i=i+1) {
		for (int j=1;j<size_of_forest-1;j=j+1) {
			if (forest[i][j] > forest[i-1][j] && visible[i-1][j] % VISIBLE_UP == 0) {
				visible[i][j] = visible[i][j]*VISIBLE_UP;
			}
		}
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
	char curr;
	int size_of_forest = 0;

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
	// Do some pointer math to setup our arrays properly
	for (int i=0;i<size_of_forest;i=i+1) {
		forest[i] = (int *)((int *)(forest+size_of_forest)+size_of_forest*i);
		visible[i] = (int *)((int *)(visible+size_of_forest)+size_of_forest*i);
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
	// Set our outside boundaries to visible from their respective directions
	for(int i=0;i<size_of_forest;i=i+1) {
		visible[0][i] = visible[0][i]*VISIBLE_UP;
		visible[i][0] = visible[i][0]*VISIBLE_LEFT;
		visible[i][size_of_forest-1] = visible[i][size_of_forest-1]*VISIBLE_RIGHT;
		visible[size_of_forest-1][i] = visible[size_of_forest-1][i]*VISIBLE_DOWN;
	}
	up(forest, visible, size_of_forest);
	// print out visible
	for (int i=0;i<size_of_forest;i=i+1) {
		for (int j=0;j<size_of_forest;j=j+1) {
			printf("%d", visible[i][j]);
		}
		printf("\n");
	}

	// Solve part one

	// Free all the memory we allocated
	free(forest);
	free(visible);

	fclose(in_file);
	return EXIT_SUCCESS;
}
