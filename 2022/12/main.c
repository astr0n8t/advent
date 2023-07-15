#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct point {
	char vis;
	char lvl;
	int depth;
	int x;
	int y;
} point;

int path(struct point** maze, int h, int w, struct point start, struct point end) {
	int depth;
	struct point* queue = (struct point*)malloc(sizeof(struct point)*h*w);
	int curr = 0;
	int size = 1;
	int x;
	int y;

	queue[curr] = start;
	queue[curr].depth = 0;

	while (queue[curr].x != end.x || queue[curr].y != end.y) {
		x = queue[curr].x;
		y = queue[curr].y;
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

		if (maze[y][x].vis != 't') {
			// right
			if (x+1 < w && (maze[y][x+1].lvl-1 <= maze[y][x].lvl)) {
				queue[size] = maze[y][x+1];
				queue[size].depth = queue[curr].depth + 1;
				size = size + 1;
			}
			// left
			if (x-1 > -1 && (maze[y][x-1].lvl-1 <= maze[y][x].lvl)) {
				queue[size] = maze[y][x-1];
				queue[size].depth = queue[curr].depth + 1;
				size = size + 1;
			}
			// up
			if (y-1 > -1 && (maze[y-1][x].lvl-1 <= maze[y][x].lvl)) {
				queue[size] = maze[y-1][x];
				queue[size].depth = queue[curr].depth + 1;
				size = size + 1;
			}
			// down
			if (y+1 < h && (maze[y+1][x].lvl-1 <= maze[y][x].lvl)) {
				queue[size] = maze[y+1][x];
				queue[size].depth = queue[curr].depth + 1;
				size = size + 1;
			}
			maze[y][x].vis = 't';
		}
		if (curr+1 < size) {
			curr = curr + 1;
		}
		else {
			// Compact array
			for (int i=0; i<size-curr; i=i+1) {
				queue[i] = queue[curr+i];
			}
			size = size-curr;
			curr = 0;
		}

	}
	depth = queue[curr].depth;
	free(queue);
	return depth;
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
	char buffer[500];
	struct point** maze;
	int height = 0;
	int width = 0;
	struct point start;
	struct point end;

	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		width = strlen(buffer) - 1;
		height = height + 1;
	}
	rewind(in_file);

	maze = (struct point**)malloc(sizeof(struct point*)*height+sizeof(struct point)*height*width);
	for (int i=0;i<height;i=i+1) {
		maze[i] = (struct point *)((struct point *)(maze+height)+width*i);
		fgets(buffer, sizeof(buffer), in_file);
		for (int j=0; j<width; j=j+1) {
			maze[i][j].x = j;
			maze[i][j].y = i;
			maze[i][j].vis = 'f';
			if (buffer[j] == 'S') {
				maze[i][j].lvl = 'a';
				start = maze[i][j];
			}
			else if (buffer[j] == 'E') {
				maze[i][j].lvl = 'z';
				end = maze[i][j];
			}
			else {
				maze[i][j].lvl = buffer[j];
			}


		}
	}
	// Update start and end
	start = maze[start.y][start.x];
	end = maze[end.y][end.x];


	printf("Minimum number of moves: %d\n", path(maze, height, width, start, end));

	// Free our memory
	free(maze);

	fclose(in_file);
	return EXIT_SUCCESS;
}
