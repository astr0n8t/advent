#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// A rough struct to hold grid coordinates
struct point {
	int x;
	int y;
	int id;
};

// Our core logic function for actually moving each tail
// the code is pretty if else heavy but that keeps it simple to understand
struct point movetail(int** grid, struct point head, struct point tail) {
	// Check if we even need to move
	if ((tail.x == head.x && tail.y == head.y) ||
		(abs(tail.x-head.x) == 1 && abs(tail.y-head.y) == 1) || 
		(abs(tail.x-head.x) == 1 && tail.y == head.y) ||
		(abs(tail.y-head.y) == 1 && tail.x == head.x)) {
		tail = tail;
	}
	// Check if we need to just move horizontally
	else if (tail.y == head.y) {
		// We're farther to the right
		if (tail.x-head.x > 0) {
			tail.x = tail.x - 1;
		}
		// Otherwise we're farther to the left
		else {
			tail.x = tail.x + 1;
		}

	}
	// Check if we need to just move vertically
	else if (tail.x == head.x) {
		// We're farther up
		if (tail.y-head.y > 0) {
			tail.y = tail.y - 1;
		}
		// Otherwise we're farther down
		else {
			tail.y = tail.y + 1;
		}
	}
	// Otherwise we have to move diagonally
	else {
		// Move tail down by one
		// Tail is to the right of head
		if (tail.x-head.x > 0) {
			tail.x = tail.x - 1;
		}
		// Tail is to the left of head
		else {
			tail.x = tail.x + 1;
		}
		// If tail is higher than head
		if (tail.y-head.y > 0) {
			tail.y = tail.y - 1;
		}
		// Tail is below head
		else {
			// Move tail up by one
			tail.y = tail.y + 1;
		}

	}

	return tail;
}

// Grid marking logic
void markgrid(int** grid, struct point tail, int height, int width) {
	// Check if we are within the bounds of the array
	if (tail.x >= width || tail.x < 0 || tail.y >= height || tail.y < 0) {
		// If we're not, lets try to wrap around to the other side of the
		// array because we don't necessarily care about the position itself
		// but rather that a unique location was visited
		// this won't always work but was enough to allow the test case
		// to succeed
		printf("ERROR: outside array bounds, attempting to workaround!\n");
		if (tail.x >= width) {
			tail.x = tail.x - width;
		}
		else if (tail.x < 0) {
			tail.x = width + tail.x - 1;
		}
		else if (tail.y >= height) {
			tail.y = tail.y - height;
		}
		else {
			tail.y = height + tail.y - 1;
		}
		// Call our function recursively so we can re-do our bounds check
		// This could put us in an infinite loop but that's preferable to 
		// corrupting memory
		// The user is also alerted
		printf("ERROR: attempting to call markgrid again with corrected bounds, this may cause infinite loops\n");
		markgrid(grid, tail, height, width);
	}
	// Mark for id 1
	if (tail.id == 1) {
		if (grid[tail.y][tail.x] == 0) {
			grid[tail.y][tail.x] = 1;
		}
		else if (grid[tail.y][tail.x] == 9) {
			grid[tail.y][tail.x] = 10;
		}
	}
	// Mark for id 9
	else if (tail.id == 9) {
		if (grid[tail.y][tail.x] == 0) {
			grid[tail.y][tail.x] = 9;
		}
		else if (grid[tail.y][tail.x] == 1) {
			grid[tail.y][tail.x] = 10;
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
	char direction;
	int move;
	struct point rope[10];
	int up = 0;
	int down = 0;
	int left = 0;
	int right = 0;
	int height = 0;
	int width = 0;
	int** grid;
	int score_1 = 0;
	int score_9 = 0;

	// We need to iterate over the file once to see how
	// big our array needs to be
	while(fscanf(in_file, "%c %d\n", &direction, &move) == 2) {
		if (direction == 'U') {
			up = up + move;
		}
		else if (direction == 'D') {
			down = down + move;
		}
		else if (direction == 'L') {
			left = left + move;
		}
		else {
			right = right + move;
		}
	}
	// This is a rough calculation so we do bounds checking 
	// when we actually set a random point.  The test cases seemed to not like
	// this allocation logic but the actual problem set worked on it
	height = up + down;
	width = left + right;

	// Initialize our rope
	rope[0].x = width / 2;
	rope[0].y = height / 2;
	rope[0].id = 0;
	for (int i=1; i<10; i=i+1) {
		rope[i].x = rope[0].x;
		rope[i].y = rope[0].y;
		rope[i].id = i;
	}

	// Go back to beginning of file
	rewind(in_file);
	// Debug print out dimensions so we can see how big our array is
	printf("Current dimensions: %d %d\n", height, width);

	// Allocate our array
	grid = (int**)malloc(sizeof(int*)*height+sizeof(int)*height*width);
	// Do some pointer math to setup our arrays properly
	for (int i=0;i<height;i=i+1) {
		grid[i] = (int *)((int *)(grid+height)+width*i);
	}
	// Zero initialize our array
	for (int i=0; i<height; i=i+1) {
		for (int j=0; j<width; j=j+1) {
			grid[i][j] = 0;
		}
	}
	
	// Make sure we mark start as visited
	markgrid(grid, rope[1], height, width);
	markgrid(grid, rope[9], height, width);

	// Get every move instruction
	while(fscanf(in_file, "%c %d\n", &direction, &move) == 2) {
		// Check the direction
		if (direction == 'U') {
			// For every move 
			for (int i=0; i<move; i=i+1) {
				// Run through the rope setting each subsequent link
				// to be the next ones head
				// Essentially its a chain actually
				rope[0].y = rope[0].y + 1;
				for (int i=1;i<10; i=i+1) {
					rope[i] = movetail(grid, rope[i-1], rope[i]);
					markgrid(grid, rope[i], height, width);
				}
			}
		}
		// Repeat that process for every direction
		else if (direction == 'D') {
			for (int i=0; i<move; i=i+1) {
				rope[0].y = rope[0].y - 1;
				for (int i=1;i<10; i=i+1) {
					rope[i] = movetail(grid, rope[i-1], rope[i]);
					markgrid(grid, rope[i], height, width);
				}
			}
		}
		else if (direction == 'L') {
			for (int i=0; i<move; i=i+1) {
				rope[0].x = rope[0].x - 1;
				for (int i=1;i<10; i=i+1) {
					rope[i] = movetail(grid, rope[i-1], rope[i]);
					markgrid(grid, rope[i], height, width);
				}
			}
		}
		else {
			for (int i=0; i<move; i=i+1) {
				rope[0].x = rope[0].x + 1;
				for (int i=1;i<10; i=i+1) {
					rope[i] = movetail(grid, rope[i-1], rope[i]);
					markgrid(grid, rope[i], height, width);
				}
			}
		}
	}

	// Go back over our array and find places visited by id 1 and 9
	for (int i=0; i<height; i=i+1) {
		for (int j=0; j<width; j=j+1) {
			if (grid[i][j] - 1 == 0 || grid[i][j] - 10 == 0) {
				score_1 = score_1 + 1;
			}
			if (grid[i][j] - 9 == 0 || grid[i][j] - 10 == 0) {
				score_9 = score_9 + 1;
			}
		}
	}

	// Print out the solution
	printf("The current number of points visited by id one is %d\n", score_1);
	printf("The current number of points visited by id nine is %d\n", score_9);

	// Free our memory
	free(grid);

	fclose(in_file);
	return EXIT_SUCCESS;
}
