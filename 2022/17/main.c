#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

// Define values
#define TYPE1 0
#define TYPE2 1
#define TYPE3 2
#define TYPE4 3
#define TYPE5 4

#define FALL 1
#define REST 0

#define RIGHT '>'
#define LEFT '<'

struct Point {
	int x;
	int y;
};

// Define a rock
struct Rock {
	struct Point coords;
	int type;
	int state;
};

// loose String struct with size
struct String {
	char* array;
	size_t size;
};

struct Round {
	int round;
	int height;
};

// Check if the coords are equal to the point
int eq(int x, int y, struct Point b) {
	return (x==b.x&&y==b.y) ? 1 : 0;
}

// Check if the Rock is touching that point
int touchpoint(struct Rock r, struct Point p) {
	int ret = 0;
	switch (r.type) {
		case TYPE1:
			ret = eq(r.coords.x, r.coords.y, p);
			ret = ret + eq(r.coords.x+1, r.coords.y, p);
			ret = ret + eq(r.coords.x+2, r.coords.y, p);
			ret = ret + eq(r.coords.x+3, r.coords.y, p);
			break;
		case TYPE2:
			ret = eq(r.coords.x+1, r.coords.y, p);
			ret = ret + eq(r.coords.x,   r.coords.y-1, p);
			ret = ret + eq(r.coords.x+1, r.coords.y-1, p);
			ret = ret + eq(r.coords.x+2, r.coords.y-1, p);
			ret = ret + eq(r.coords.x+1, r.coords.y-2, p);
			break;
		case TYPE3:
			ret = eq(r.coords.x+2, r.coords.y, p);
			ret = ret + eq(r.coords.x+2, r.coords.y-1, p);
			ret = ret + eq(r.coords.x,   r.coords.y-2, p);
			ret = ret + eq(r.coords.x+1, r.coords.y-2, p);
			ret = ret + eq(r.coords.x+2, r.coords.y-2, p);
			break;
		case TYPE4:
			ret = eq(r.coords.x, r.coords.y, p);
			ret = ret + eq(r.coords.x, r.coords.y-1, p);
			ret = ret + eq(r.coords.x, r.coords.y-2, p);
			ret = ret + eq(r.coords.x, r.coords.y-3, p);
			break;
		case TYPE5:
			ret = eq(r.coords.x, r.coords.y, p);
			ret = ret + eq(r.coords.x+1, r.coords.y, p);
			ret = ret + eq(r.coords.x,   r.coords.y-1, p);
			ret = ret + eq(r.coords.x+1, r.coords.y-1, p);
			break;
	}
	return ret;
}

int touchrock(struct Rock a, struct Rock b) {
	int ret = 0;
	struct Point tmp;
	switch (b.type) {
		case TYPE1:
			tmp.x = b.coords.x;
			tmp.y = b.coords.y;
			ret = touchpoint(a, tmp);
			tmp.x = b.coords.x+1;
			ret = ret + touchpoint(a, tmp);
			tmp.x = b.coords.x+2;
			ret = ret + touchpoint(a, tmp);
			tmp.x = b.coords.x+3;
			ret = ret + touchpoint(a, tmp);
			break;
		case TYPE2:
			tmp.x = b.coords.x+1;
			tmp.y = b.coords.y;
			ret = touchpoint(a, tmp);
			tmp.x = b.coords.x;
			tmp.y = b.coords.y-1;
			ret = ret + touchpoint(a, tmp);
			tmp.x = b.coords.x+1;
			ret = ret + touchpoint(a, tmp);
			tmp.x = b.coords.x+2;
			ret = ret + touchpoint(a, tmp);
			tmp.x = b.coords.x+1;
			tmp.y = b.coords.y-2;
			ret = ret + touchpoint(a, tmp);
			break;
		case TYPE3:
			tmp.x = b.coords.x+2;
			tmp.y = b.coords.y;
			ret = touchpoint(a, tmp);
			tmp.y = b.coords.y-1;
			ret = ret + touchpoint(a, tmp);
			tmp.y = b.coords.y-2;
			ret = ret + touchpoint(a, tmp);
			tmp.x = b.coords.x+1;
			ret = ret + touchpoint(a, tmp);
			tmp.x = b.coords.x;
			ret = ret + touchpoint(a, tmp);
			break;
		case TYPE4:
			tmp.x = b.coords.x;
			tmp.y = b.coords.y;
			ret = touchpoint(a, tmp);
			tmp.y = b.coords.y-1;
			ret = ret + touchpoint(a, tmp);
			tmp.y = b.coords.y-2;
			ret = ret + touchpoint(a, tmp);
			tmp.y = b.coords.y-3;
			ret = ret + touchpoint(a, tmp);
			break;
		case TYPE5:
			tmp.x = b.coords.x;
			tmp.y = b.coords.y;
			ret = touchpoint(a, tmp);
			tmp.x = b.coords.x+1;
			ret = ret + touchpoint(a, tmp);
			tmp.x = b.coords.x;
			tmp.y = b.coords.y-1;
			ret = ret + touchpoint(a, tmp);
			tmp.x = b.coords.x+1;
			ret = ret + touchpoint(a, tmp);
			break;
	}
	return ret;
}
	


// Returns highest height of the bottom
int highest(struct Rock* history, int curr) {
	int h = 0;
	for (int i=0; i<curr; i++) {
		if (history[i].coords.y > h) {
			h = history[i].coords.y;
		}
	}
	return h;
}
struct Rock left(struct Rock c, struct Rock* history, int curr) {
	int move = 0;
	struct Point leftb = {.x=0};
	struct Rock tmp;
	leftb.y = c.coords.y;

	if (!touchpoint(c, leftb)) {
		// Check y-1
		leftb.y--;
		if (!touchpoint(c, leftb)) {
			// Also check y-2 for rock type 2
			leftb.y--;
			if (!touchpoint(c, leftb)) {
				move = 1;
			}
		}
	}

	// Check if we're touching bottom pieces
	for (int i=0; i<curr; i++) {
		tmp = history[i];
		tmp.coords.x++;
		if (touchrock(c, tmp)) {
			move = 0;
		}
	}

	if (move) {
		c.coords.x--;
	}
	return c;
}

struct Rock right(struct Rock c, struct Rock* history, int curr) {
	int move = 0;
	struct Point rightb = {.x=6};
	struct Rock tmp;
	rightb.y = c.coords.y;

	if (!touchpoint(c, rightb)) {
		rightb.y--;
		if (!touchpoint(c, rightb)) {
			move = 1;
		}
	}

	// Check if we're touching bottom pieces
	for (int i=0; i<curr; i++) {
		tmp = history[i];
		tmp.coords.x--;
		if (touchrock(c, tmp)) {
			move = 0;
		}
	}

	if (move) {
		c.coords.x++;
	}
	return c;
}

struct Rock down(struct Rock c, struct Rock* history, int curr) {
	int move = 1;
	struct Rock tmp;
	static int starting_heights[5] = {1, 3, 3, 4, 2};

	if (!(c.coords.y-starting_heights[c.type])) {
		move = 0;
	}
	else {
		for (int i=0; i<curr; i++) {
			tmp = history[i];
			tmp.coords.y++;
			if (touchrock(c, tmp)) {
				move = 0;
				break;
			}
		}
	}

	if (move) {
		c.coords.y--;
	}
	else {
		c.state = REST;
	}

	return c;
}

int droprock(struct Rock* history, int i, struct String wind, int curr) {
	static int starting_heights[5] = {1, 3, 3, 4, 2};
	if (curr>0) {
		history[i].type = (history[i-1].type+1) % 5;
	}
	else {
		history[i].type = TYPE1;
	}
	history[i].coords.x = 2;
	history[i].coords.y = highest(history, i) + starting_heights[history[i].type] + 3;
	history[i].state = FALL;
	while (history[i].state != REST) {
		switch (wind.array[curr]) {
			case RIGHT:
				history[i] = right(history[i], history, i);
				break;
			case LEFT:
				history[i] = left(history[i], history, i);
				break;
		}
		curr = (curr+1) % wind.size;
		history[i] = down(history[i], history, i);

	}
	return curr;
}

int part1(struct String wind) {
	// Declare stack vars
	struct Rock history[2022];
	int curr = 0;

	// Enter our main loop
	for (int i=0; i<2022; i++) {
		curr = droprock(history, i, wind, curr);
	}

	return highest(history, 2022);
}

long part2(struct String wind) {
	long result = 0;
	long quotient = 0;
	long remainder = 0;
	int curr = 0;
	int key = 0;
	int mask = 10;
	static long num_rounds = 1000000000000;
	struct Rock* history = (struct Rock*)malloc(sizeof(struct Rock)*wind.size*5);
	struct Round* rounds = (struct Round*)malloc(sizeof(struct Round)*wind.size*5);
	for (int i=0; i<wind.size*5; i++) {
		rounds[i].round = 0;
		rounds[i].height = 0;
	}
	
	for (int i=0; i<INT_MAX && !result; i++) {
		key = (i%5)*wind.size+curr;
		if (rounds[key].height) {
			quotient = (1000000000000L - (long)i) / (long)(i-rounds[key].round);
			remainder = (1000000000000L - (long)i) % (long)(i-rounds[key].round);
			if (remainder == 0L) {
				result = highest(history, i);
				result = (long)result+(long)(result-rounds[key].height)*quotient;
			}
		}
		else {
			rounds[key].height = highest(history, i);
			rounds[key].round = i;
		}
		curr = droprock(history, i, wind, curr);
	}

	free(history);
	free(rounds);
	return result;
}

// Function to parse the input file and return
// a string representing the wind
struct String processinput(FILE *in_file) {
	struct String output = {.size = 0};
	char tmp;
	int curr = 0;

	// Get length of turns
	while((tmp = fgetc(in_file)) && !feof(in_file)) {
		if (tmp == RIGHT || tmp == LEFT) {
			output.size++;
		}
	}
	// Go back to beginning of the file
	rewind(in_file);
	output.array = (char*)malloc(sizeof(char)*output.size);

	while((tmp = fgetc(in_file)) && !feof(in_file)) {
		if (tmp == RIGHT || tmp == LEFT) {
			output.array[curr] = tmp;
			curr++;
		}
	}
	// Replace the ending newline with a terminater
	output.array[curr] = 0;

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
	struct String wind = processinput(in_file);
	// Solve part 1
	printf("The height of the rocks after 2022 rounds is %d\n", part1(wind));
	printf("The height of the rocks after 1000000000000 rounds is %ld\n", part2(wind));
	// Clean up memory
	free(wind.array);
	// Close our file
	fclose(in_file);

	return EXIT_SUCCESS;
}
