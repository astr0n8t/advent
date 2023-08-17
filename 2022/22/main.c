#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UP 0
#define RIGHT 90
#define DOWN 180
#define LEFT 270

typedef struct pathgrid {
	char** array;
	size_t width;
	size_t height;
} PathGrid;

typedef struct move {
	char direction;
	int distance;
} Move;

typedef struct movelist {
	Move* array;
	size_t size;
	size_t max;
} MoveList;

typedef struct position {
	int d;
	int x;
	int y;
} Position;

void printpos(PathGrid path, Position curr) {
	for (int i=0; i<path.height; i++) {
		for (int j=0; j<path.width-1; j++) {
			if(i == curr.y && j == curr.x) {
				printf("@");
			}
			else {
				printf("%c", path.array[i][j]);
			}
		}
		printf("\n");
	}
	printf("\n");
	return;
}

void fill(PathGrid* path) {
	for (int i=0; i<path->height; i++) {
		for (int j=0; j<path->width-1; j++) {
			if (path->array[i][j] != '.' && path->array[i][j] != '#') {
				path->array[i][j] = '0';
			}
		}
	}
	return;
}

int dadd(int direction, int sum) {
	direction += sum;
	while (direction < 0) {
		direction += 360;
	}
	while (direction >= 360) {
		direction -= 360;
	}
	return direction;
}

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


Position next(PathGrid path, int distance, Position curr) {
	int wrap = 0;
	int wallwrap = 0;
	while (distance) {
		if (wallwrap) {
			curr = backward(curr);
		}
		else {
			curr = forward(curr);
		}
		if (curr.x < 0) {
			curr.x = path.width-2;
		}
		if (curr.y < 0) {
			curr.y = path.height-1;
		}
		if (curr.y >= path.height) {
			curr.y = 0;
		}
		if (curr.x >= path.width-1) {
			curr.x = 0;
		}
		if (path.array[curr.y][curr.x] == '#') {
			if (wrap) {
				wallwrap = 1;
			}
			else {
				curr = backward(curr);
				distance = 0;
			}
		}
		else if (path.array[curr.y][curr.x] == '0') {
			if (!wrap) {
				wrap = 1;
			}
		}
		else {
			if (wallwrap) {
				distance = 0;
			}
			else {
				wrap = 0;
				distance--;
			}
		}
		printf("X: %d Y: %d D: %d\n", curr.x, curr.y, curr.d);
	}
	return curr;
}


Position solve(PathGrid path, MoveList moves) {
	// Set up our starting point
	Position curr = {.x=0,.y=0,.d=RIGHT};
	for (int i=0; i<path.width && !curr.x; i++) {
		if (path.array[0][i] == '.') {
			curr.x = i;
		}
	}
	for (int i=0; i<moves.size; i++) {
		switch (moves.array[i].direction) {
			case 'R':
				curr.d = dadd(curr.d, 90);
				break;
			case 'L':
				curr.d = dadd(curr.d, -90);
				break;
		}

		curr = next(path, moves.array[i].distance, curr);
	}

	return curr;
}


// Solve part1
int part1(PathGrid path, MoveList moves) {
	int answer = 0;
	Position final = solve(path, moves);
	printf("X: %d Y: %d D: %d\n", final.x, final.y, final.d);
	answer = 1000*(final.y+1) + 4*(final.x+1);
	switch (final.d) {
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

	return answer;
}

// Solve part2
long part2() {
	return 0;
}

// Processes the selected input file and stores the result in the 
void processinput(char* filename, PathGrid* path, MoveList* moves) {
	// Read in our file
	FILE *in_file = fopen(filename, "r");
	if (in_file == NULL) {
		printf("Could not open input file\n");
		exit(EXIT_FAILURE);
	}

	// Stack var
	char buffer[500];
	size_t tmp = 0;
	int curr = 0;

	// Figure out how many monkeys there are
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
	fill(&path);

	// Solve part1
	printf("The password for part1 is %d\n", part1(path, moves));
	// Solve part2
	printf("The value for humn should be %ld\n", part2());

	// Free our memory
	free(moves.array);
	free(path.array);

	return EXIT_SUCCESS;
}
