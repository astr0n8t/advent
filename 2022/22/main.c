#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Direction definitions
#define UP 0
#define RIGHT 90
#define DOWN 180
#define LEFT 270

// Define path 2D list
typedef struct pathgrid {
	char** array;
	size_t width;
	size_t height;
} PathGrid;

// Define a move
typedef struct move {
	char direction;
	int distance;
} Move;

// Define a list of moves
typedef struct movelist {
	Move* array;
	size_t size;
	size_t max;
} MoveList;

// Define a position
// which is really a vector
typedef struct position {
	int d;
	int x;
	int y;
} Position;

// This function replaces all spaces with zeros
// because its easier to test on
void fill(PathGrid* path) {
	for (int i=0; i<path->height; i++) {
		for (int j=0; j<path->width-1; j++) {
			// If its not a period or a hashtag
			if (path->array[i][j] != '.' && path->array[i][j] != '#') {
				// Replace it with a zero
				path->array[i][j] = '0';
			}
		}
	}
	return;
}

// Moves the position forward in the
// direction its moving
Position forward(Position curr) {
	switch(curr.d) {
		case UP:
			curr.y--;
			break;
		case DOWN:
			curr.y++;
			break;
		case LEFT:
			curr.x--;
			break;
		case RIGHT:
			curr.x++;
			break;
	}
	return curr;
}

// Moves the position backward in the
// direction its moving
Position backward(Position curr) {
	switch(curr.d) {
		case UP:
			curr.y++;
			break;
		case DOWN:
			curr.y--;
			break;
		case LEFT:
			curr.x++;
			break;
		case RIGHT:
			curr.x--;
			break;
	}
	return curr;
}

// Walks along the edges of the cubes
Position walkcubeedge(PathGrid path, Position curr, int direction) {
	// Move back one to get back into the array or zone
	curr = backward(curr);
	// Hardcoded representation of the zones of our map
	static int map[4][3] = {
		{0, 1, 2},
		{0, 3, 0},
		{4, 5, 0},
		{6, 0, 0}};
	// FIgure out what zone we're in
	int zone = map[curr.y/50][curr.x/50];
	// Make a copy of our current position
	Position original = curr;
	switch (zone) {
		case 1:
			if (direction == UP) {
				// Wrap to 6
				curr.x = 0;
				curr.y = original.x + 100;
				curr.d = RIGHT;

			}
			else if (direction == LEFT) {
				// Wrap to 4
				curr.x = 0;
				curr.y = 149-original.y;
				curr.d = RIGHT;
			}
			break;
		case 2:
			if (direction == UP) {
				// Wrap to 6
				curr.x = original.x-100;
				curr.y = 199;
				curr.d = UP;
			}
			else if (direction == DOWN) {
				// Wrap to 3
				curr.x = 99;
				curr.y = original.x - 50;
				curr.d = LEFT;
			}
			else if (direction == RIGHT) {
				// Wrap to 5
				curr.x = 99;
				curr.y = 149-original.y;
				curr.d = LEFT;
			}
			break;
		case 3:
			if (direction == LEFT) {
				// Wrap to 4
				curr.x = original.y-50;
				curr.y = 100;
				curr.d = DOWN;
			}
			else if (direction == RIGHT) {
				// Wrap to 2
				curr.x = original.y + 50;
				curr.y = 49;
				curr.d = UP;
			}
			break;
		case 4:
			if (direction == LEFT) {
				// Wrap to 1
				curr.x = 50;
				curr.y = 149 - original.y;
				curr.d = RIGHT;
			}
			else if (direction == UP) {
				// Wrap to 3
				curr.x = 50;
				curr.y = original.x + 50;
				curr.d = RIGHT;
			}
			break;
		case 5:
			if (direction == RIGHT) {
				// Wrap to 2
				curr.x = 149;
				curr.y = 149-original.y;
				curr.d = LEFT;
			}
			else if (direction == DOWN) {
				// Wrap to 6
				curr.x = 49;
				curr.y = original.x + 100;
				curr.d = LEFT;
			}
			break;
		case 6:
			if (direction == LEFT) {
				// Wrap to 1
				curr.x = original.y - 100;
				curr.y = 0;
				curr.d = DOWN;
			}
			else if (direction == DOWN) {
				// Wrap to 2
				curr.x = original.x + 100;
				curr.y = 0;
				curr.d = DOWN;
			}
			else if (direction == RIGHT) {
				// Wrap to 5
				curr.x = original.y - 100;
				curr.y = 149;
				curr.d = UP;
			}
			break;
	}

	// Return the wrapped position
	return curr;
}

// This function figures out which direction we need to wrap
// and then calls walkcubeedge
Position cubewrap(PathGrid path, Position curr) {
	// Stack var
	int wrap = -1;
	// Check if we walked off the left edge
	if (curr.x < 0) {
		wrap = LEFT;
	}
	// Check if we walked off the top edge
	else if (curr.y < 0) {
		wrap = UP;
	}
	// Check if we walked off the bottom edge
	else if (curr.y >= path.height) {
		wrap = DOWN;
	}
	// Check if we walked off the right edge
	else if (curr.x >= path.width-1) {
		wrap = RIGHT;
	}
	// Otherwise check if we're not in a zone
	else if (path.array[curr.y][curr.x] == '0') {
		// Set the wrap to our current direction
		wrap = curr.d;
	}
	// Check if we need to wrap at all
	if (wrap != -1) {
		// If we do call walkcubeedge
		curr = walkcubeedge(path, curr, wrap);
	}
	// Return the wrapped position
	return curr;
}

// Wraps normally around the array
Position normalwrap(PathGrid path, Position curr) {
	// Check if we walked off the left edge
	if (curr.x < 0) {
		curr.x = path.width-2;
	}
	// Check if we walked off the top edge
	else if (curr.y < 0) {
		curr.y = path.height-1;
	}
	// Check if we walked off the bottom edge
	else if (curr.y >= path.height) {
		curr.y = 0;
	}
	// Check if we walked off the right edge
	else if (curr.x >= path.width-1) {
		curr.x = 0;
	}
	// Check if we walked outside our zone
	else if (path.array[curr.y][curr.x] == '0') {
		// Set the correct value based on the direction
		if (curr.d == LEFT) {
			curr.x = path.width-2;
		}
		else if (curr.d == UP) {
			curr.y = path.height-1;
		}
		else if (curr.d == DOWN) {
			curr.y = 0;
		}
		else if (curr.d == RIGHT) {
			curr.x = 0;
		}
	}
	// Go forward until we're in a zone
	while (path.array[curr.y][curr.x] == '0') {
		curr = forward(curr);
	}
	// Return the new position
	return curr;
}

// Get the next position given a path, distance,  current position, and a wrap function
Position next(PathGrid path, int distance, Position curr, Position wrapfunc(PathGrid, Position)) {
	// Stack vars
	Position original;
	// Iterate until we've gone the full distance
	while (distance) {
		// Go forward one
		curr = forward(curr);
		// Make a copy of our current position
		original = curr;
		// Calculate the wrapped position, if we don't need to wrap
		// there will be no change
		curr = wrapfunc(path, curr);
		// Check if we hit a wall
		if (path.array[curr.y][curr.x] == '#') {
			// If we did, check if we just wrapped
			if (curr.x != original.x || curr.y != original.y) {
				// If we wrapped, then go back to where we were
				// before we wrapped
				curr = original;
			}
			// Go back one to move off the wall or back into the zone
			curr = backward(curr);
			// We can't go any farther so exit now
			distance = 0;
		}
		// Otherwise we've gone one unit of distance forward
		else {
			distance--;
		}
	}
	// Return the new position
	return curr;
}

// Solution function that uses a path, list of moves, and wrap function to
// compute the answer based on end position
int solve(PathGrid path, MoveList moves, Position wrapfunc(PathGrid, Position)) {
	// Stack vars
	int answer = 0;
	Position curr = {.x=0,.y=0,.d=RIGHT};
	// Calculate our starting position
	for (int i=0; i<path.width && !curr.x; i++) {
		if (path.array[0][i] == '.') {
			curr.x = i;
		}
	}
	// Iteratee for every move
	for (int i=0; i<moves.size; i++) {
		// Turn to our current direction
		switch (moves.array[i].direction) {
			case 'R':
				curr.d += 90;
				break;
			case 'L':
				curr.d -= 90;
				break;
		}
		// Keep our direction to one of the four values
		while (curr.d < 0) {
			curr.d += 360;
		}
		while (curr.d >= 360) {
			curr.d -= 360;
		}

		// Calculate the position after this move
		curr = next(path, moves.array[i].distance, curr, wrapfunc);
	}
	// Calculate our answer without the direction
	answer = 1000*(curr.y+1) + 4*(curr.x+1);
	// Add the constant based on direction
	switch (curr.d) {
		case DOWN:
			answer += 1;
			break;
		case LEFT:
			answer += 2;
			break;
		case UP:
			answer += 3;
			break;
	}
	// Return our answer
	return answer;
}

// Solve part1
int part1(PathGrid path, MoveList moves) {
	// Use normal wrap to solve
	return solve(path, moves, normalwrap);
}

// Solve part2
int part2(PathGrid path, MoveList moves) {
	// Use cubewrap to solve
	return solve(path, moves, cubewrap);
}

// Processes the selected input file and stores the result in the 
// given PathGrid and MoveList
void processinput(char* filename, PathGrid* path, MoveList* moves) {
	// Read in our file
	FILE *in_file = fopen(filename, "r");
	if (in_file == NULL) {
		printf("Could not open input file\n");
		exit(EXIT_FAILURE);
	}

	// Stack vars
	char buffer[500];
	size_t tmp = 0;
	int curr = 0;

	// Figure out the dimensions of the path
	while(fgets(buffer, sizeof buffer, in_file) != NULL) {
		// End on first newline
		if (buffer[0] == '\n') {
			break;
		}

		// Get the length of the current line
		tmp = strlen(buffer);
		// Check if the length is longer than our width
		if (tmp > path->width) {
			path->width = tmp;
		}
		// Increment our height
		path->height++;
	}
	// Go back to the beginning of the file
	rewind(in_file);

	// Allocate our 2D path array
	path->array = malloc((sizeof *path->array)*path->height + (sizeof **path->array)*path->height*path->width);
	for (int i=0; i<path->height; i++) {
		path->array[i] = (char*)((char*)(path->array+path->height)+path->width*i);
	}
	// Go back over the file line by line
	while(fgets(buffer, sizeof buffer, in_file) != NULL) {
		// End on first newline
		if (buffer[0] == '\n') {
			break;
		}
		// Copy the buffer into the array
		strncpy(path->array[curr], buffer, path->width-1);
		// Terminate the string on the first newline
		for (int i=0; i<path->width; i++) {
			if (path->array[curr][i] == '\n') {
				path->array[curr][i] = 0;
			}
		}
		curr++;
	}

	// Initialize and allocate our moves array
	moves->size = 0;
	moves->max = 100;
	moves->array = malloc((sizeof *moves->array)*moves->max);
	// Go over the file a line at a time
	fscanf(in_file, "%d", &moves->array[moves->size].distance);
	moves->array[moves->size].direction = 0;
	moves->size++;
	while(fscanf(in_file, "%c%d", &moves->array[moves->size].direction,
				&moves->array[moves->size].distance)==2) {
		// Increment our size and check if we hit our max
		if (++moves->size == moves->max) {
			// Add another 100 values
			moves->max += 100;
			// Re-allocate our array
			moves->array = realloc(moves->array, (sizeof *moves->array)*moves->max);
		}
	}
	// Close our file in case we want to call this function again
	fclose(in_file);
	return;
}

int main(int argc, char *argv[])
{
	// Stack vars
	MoveList moves = {.size=0, .max=0, .array=NULL};
	PathGrid path = {.width=0, .height=0, .array=NULL};
	// Get our monkey input
	processinput("input.txt", &path, &moves);
	// Fill our path with zeros
	fill(&path);

	// Solve part1
	printf("The password for part1 is %d\n", part1(path, moves));
	// Solve part2
	printf("The password for part2 is %d\n", part2(path, moves));

	// Free our memory
	free(moves.array);
	free(path.array);

	return EXIT_SUCCESS;
}
