#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Define our point struct
struct point {
	char vis;
	char lvl;
	int depth;
	int x;
	int y;
} point;

// Solve for shortest length path from start to end
int path(struct point** maze, int h, int w, struct point start, struct point end) {
	// Stack vars
	int depth;
	int curr = 0;
	int size = 1;
	int x;
	int y;
	// queue array
	struct point* queue = (struct point*)malloc(sizeof(struct point)*h*w);

	// Start at start
	queue[curr] = start;
	queue[curr].depth = 0;

	// Iterate until we have hit end
	while (queue[curr].x != end.x || queue[curr].y != end.y) {
		// Set our position to the current point
		x = queue[curr].x;
		y = queue[curr].y;
		// Set our depth to the current depth
		depth = queue[curr].depth;

		if (size+4 > w*h) {
			// Compact array
			for (int i=0; i<size-curr; i=i+1) {
				queue[i] = queue[curr+i];
			}
			size = size-curr;
			curr = 0;
			if (size+4 > w*h) {
				// Increase array size as last resort
				queue = (struct point*)realloc(queue, sizeof(struct point)*size*2);
			}
		}

		// Check if we haven't visited this point yet
		if (maze[y][x].vis != 't') {
			// Check if we can go right, if so add to the queue
			if (x+1 < w && (maze[y][x+1].lvl-1 <= maze[y][x].lvl)) {
				queue[size] = maze[y][x+1];
				queue[size].depth = depth + 1;
				size = size + 1;
			}
			// Check if we can go left, if so add to the queue
			if (x-1 > -1 && (maze[y][x-1].lvl-1 <= maze[y][x].lvl)) {
				queue[size] = maze[y][x-1];
				queue[size].depth = depth + 1;
				size = size + 1;
			}
			// Check if we can go up, if so add to the queue
			if (y-1 > -1 && (maze[y-1][x].lvl-1 <= maze[y][x].lvl)) {
				queue[size] = maze[y-1][x];
				queue[size].depth = depth + 1;
				size = size + 1;
			}
			// Check if we can go down, if so add to the queue
			if (y+1 < h && (maze[y+1][x].lvl-1 <= maze[y][x].lvl)) {
				queue[size] = maze[y+1][x];
				queue[size].depth = depth + 1;
				size = size + 1;
			}
			// Mark point as visited
			maze[y][x].vis = 't';
		}
		// Check if we've exhausted the queue
		// in which case there is no path
		if (curr == size-1) {
			return EXIT_FAILURE;
		}
		// Otherwise continue down the queue
		else {
			curr = curr + 1;
		}
	}
	// Reset the maze for re-use
	for (int y=0;y<h;y=y+1) {
		for (int x=0; x<w;x=x+1) {
			maze[y][x].vis = 'f';
		}
	}
	// Our overall depth is the depth of curr which should be end
	depth = queue[curr].depth;
	// Free our memory
	free(queue);
	// Return the depth
	return depth;
}

// Solve for the best starting position with shortest depth
int bstart(struct point** maze, int h, int w, struct point end) {
	// Set stack vars
	int depth = 0;
	int tmp = 0;
	// Iterate over maze
	for (int y=0;y<h;y=y+1) {
		for (int x=0;x<w;x=x+1) {
			// Try every potential starting point
			if (maze[y][x].lvl == 'a') {
				// Get the depth of the path
				tmp = path(maze, h, w, maze[y][x], end);
				// If the path exists, and either depth is not set
				// or our depth is better set the current depth to the new depth
				if (tmp != EXIT_FAILURE && (!depth || tmp < depth)) {
					depth = tmp;
				}
			}
		}
	}
	// Return the best depth
	return depth;
}
		
int main(int argc, char *argv[])
{
	// Read in our input file
	FILE *in_file = fopen("input.txt", "r");
	if (in_file == NULL) {
		printf("Could not open input file!");
		return EXIT_FAILURE;
	}
	// Declare our stack vars
	char buffer[500];
	struct point** maze;
	int height = 0;
	int width = 0;
	struct point start;
	struct point end;

	// Get the dimensions of our maze
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		width = strlen(buffer) - 1;
		height = height + 1;
	}
	// Go back to the beginning of the file
	rewind(in_file);

	// Allocate our maze memory
	maze = (struct point**)malloc(sizeof(struct point*)*height+sizeof(struct point)*height*width);
	for (int i=0;i<height;i=i+1) {
		// Set our inner pointers
		maze[i] = (struct point *)((struct point *)(maze+height)+width*i);
		// Get each line of the file
		fgets(buffer, sizeof(buffer), in_file);
		// For each character
		for (int j=0; j<width; j=j+1) {
			// Initialize our point
			maze[i][j].x = j;
			maze[i][j].y = i;
			maze[i][j].vis = 'f';
			// Check if this is start
			if (buffer[j] == 'S') {
				maze[i][j].lvl = 'a';
				start = maze[i][j];
			}
			// Check if this is end
			else if (buffer[j] == 'E') {
				maze[i][j].lvl = 'z';
				end = maze[i][j];
			}
			// Otherwise just set it like normal
			else {
				maze[i][j].lvl = buffer[j];
			}


		}
	}
	// Update start and end
	start = maze[start.y][start.x];
	end = maze[end.y][end.x];

	// Solve part one
	printf("Minimum number of moves: %d\n", path(maze, height, width, start, end));
	// Solve part two
	printf("Minimum number of moves from better start: %d\n", bstart(maze, height, width, end));

	// Free our memory
	free(maze);
	// Close our file
	fclose(in_file);
	return EXIT_SUCCESS;
}
