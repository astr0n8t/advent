#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

// Define constant values
#define TYPE1 0
#define TYPE2 1
#define TYPE3 2
#define TYPE4 3
#define TYPE5 4

#define FALL 1
#define REST 0

#define RIGHT '>'
#define LEFT '<'

// Basic Point struct
struct Point {
	int x;
	int y;
};

// Define a rock struct
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

// struct to hold round info
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
		// For shape -
		case TYPE1:
			ret = eq(r.coords.x, r.coords.y, p);
			ret = ret + eq(r.coords.x+1, r.coords.y, p);
			ret = ret + eq(r.coords.x+2, r.coords.y, p);
			ret = ret + eq(r.coords.x+3, r.coords.y, p);
			break;
		// For shape +
		case TYPE2:
			ret = eq(r.coords.x+1, r.coords.y, p);
			ret = ret + eq(r.coords.x,   r.coords.y-1, p);
			ret = ret + eq(r.coords.x+1, r.coords.y-1, p);
			ret = ret + eq(r.coords.x+2, r.coords.y-1, p);
			ret = ret + eq(r.coords.x+1, r.coords.y-2, p);
			break;
		// For shape _|
		case TYPE3:
			ret = eq(r.coords.x+2, r.coords.y, p);
			ret = ret + eq(r.coords.x+2, r.coords.y-1, p);
			ret = ret + eq(r.coords.x,   r.coords.y-2, p);
			ret = ret + eq(r.coords.x+1, r.coords.y-2, p);
			ret = ret + eq(r.coords.x+2, r.coords.y-2, p);
			break;
		// For shape |
		case TYPE4:
			ret = eq(r.coords.x, r.coords.y, p);
			ret = ret + eq(r.coords.x, r.coords.y-1, p);
			ret = ret + eq(r.coords.x, r.coords.y-2, p);
			ret = ret + eq(r.coords.x, r.coords.y-3, p);
			break;
		// For shape []
		case TYPE5:
			ret = eq(r.coords.x, r.coords.y, p);
			ret = ret + eq(r.coords.x+1, r.coords.y, p);
			ret = ret + eq(r.coords.x,   r.coords.y-1, p);
			ret = ret + eq(r.coords.x+1, r.coords.y-1, p);
			break;
	}
	return ret;
}

// Check if two rocks are touching
// We basically need to call touchpoint for 
// every point of the second rock
int touchrock(struct Rock a, struct Rock b) {
	int ret = 0;
	struct Point tmp;
	switch (b.type) {
		// For shape -
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
		// For shape +
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
		// For shape _|
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
		// For shape |
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
		// For shape []
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
	


// Returns highest height of all rocks
// pretty inefficient but its fine
int highest(struct Rock* history, int curr) {
	int h = 0;
	// Just iterate over all the rocks
	for (int i=0; i<curr; i++) {
		// If one is higher than the current
		if (history[i].coords.y > h) {
			// Set it to be the new current
			h = history[i].coords.y;
		}
	}
	// Return the highest rock
	return h;
}

// Tries to move the current rock left
struct Rock left(struct Rock c, struct Rock* history, int curr) {
	// Stack vars
	int move = 0;
	struct Point leftb = {.x=0};
	struct Rock tmp;
	// Set leftb to the same hieght as the current rock
	leftb.y = c.coords.y;

	// Check y
	if (!touchpoint(c, leftb)) {
		// Check y-1
		leftb.y--;
		if (!touchpoint(c, leftb)) {
			// Also check y-2 for rock type 2
			leftb.y--;
			if (!touchpoint(c, leftb)) {
				// If all three checks pass, then we should move left
				move = 1;
			}
		}
	}

	// Check if we're touching other rocks
	for (int i=0; i<curr && move; i++) {
		tmp = history[i];
		tmp.coords.x++;
		if (touchrock(c, tmp)) {
			// If we're touching a rock don't move
			move = 0;
		}
	}

	// If we need to move 
	if (move) {
		// Move to the left
		c.coords.x--;
	}
	// Return the current rock
	return c;
}

// Tries to move the current rock right
struct Rock right(struct Rock c, struct Rock* history, int curr) {
	// Stack vars
	int move = 0;
	struct Point rightb = {.x=6};
	struct Rock tmp;
	// Set rightb to the same height as the current rock
	rightb.y = c.coords.y;

	// Check y
	if (!touchpoint(c, rightb)) {
		// Check y-1
		rightb.y--;
		if (!touchpoint(c, rightb)) {
			// If both checks pass, then we should move right
			move = 1;
		}
	}

	// Check if we're touching bottom pieces
	for (int i=0; i<curr && move; i++) {
		tmp = history[i];
		tmp.coords.x--;
		if (touchrock(c, tmp)) {
			// If we're touching a rock don't move
			move = 0;
		}
	}

	// If we need to move
	if (move) {
		// Move to the right
		c.coords.x++;
	}
	// Return the current rock
	return c;
}

// Tries to move the current rock down
struct Rock down(struct Rock c, struct Rock* history, int curr) {
	// Stack vars
	// Assume we will move down
	int move = 1;
	struct Rock tmp;
	static int starting_heights[5] = {1, 3, 3, 4, 2};

	// If we're going to hit the bottom don't move
	if (!(c.coords.y-starting_heights[c.type])) {
		move = 0;
	}
	else {
		// Otherwise check if we're going to land on another rock
		for (int i=0; i<curr && move; i++) {
			tmp = history[i];
			tmp.coords.y++;
			if (touchrock(c, tmp)) {
				// If we are then don't move down
				move = 0;
			}
		}
	}

	// Check if we're moving down
	if (move) {
		// Move down
		c.coords.y--;
	}
	// Otherwise the current rock is at rest
	else {
		c.state = REST;
	}

	// Return the current rock
	return c;
}

// Function that runs one rock of simulation and returns the current index
// of wind direction
int droprock(struct Rock* history, int i, struct String wind, int curr) {
	static int starting_heights[5] = {1, 3, 3, 4, 2};
	// Check if we're past the first rock
	if (curr>0) {
		// If so set the current rock type to the last plus one
		// starting over if we reached the end of rock types
		history[i].type = (history[i-1].type+1) % 5;
	}
	else {
		// Otherwise just set the type to type 1
		history[i].type = TYPE1;
	}
	// Our starting x coord is always 2
	history[i].coords.x = 2;
	// Our y coord is 3 above the highest rock
	// we also need to factor in how tall our type of rock is
	history[i].coords.y = highest(history, i) + starting_heights[history[i].type] + 3;
	// Set the current rock's state to falling
	history[i].state = FALL;
	// Iterate until the current rock has come to rest
	while (history[i].state != REST) {
		// Check which direction the wind is going
		switch (wind.array[curr]) {
			case RIGHT:
				// Try to move to the right
				history[i] = right(history[i], history, i);
				break;
			case LEFT:
				// Try to move to the left
				history[i] = left(history[i], history, i);
				break;
		}
		// Calculate the new curr which is just one plus the current
		// but start over if we hit the end
		curr = (curr+1) % wind.size;
		// Try to move the current rock down
		history[i] = down(history[i], history, i);

	}
	// Return the current position of the wind array
	return curr;
}

// Solve part1
int part1(struct String wind) {
	// Declare stack vars
	// Our history will always be 2022
	struct Rock history[2022];
	int curr = 0;

	// Enter our main loop
	for (int i=0; i<2022; i++) {
		// Just drop 2022 rocks
		curr = droprock(history, i, wind, curr);
	}

	// Return the highest rock after 2022 rocks
	// our dropped
	return highest(history, 2022);
}

// Solve part2
// needs to be a long because size
long part2(struct String wind) {
	// Stack vars
	long result = 0;
	long quotient = 0;
	long remainder = 0;
	int curr = 0;
	int key = 0;
	int mask = 10;
	// Our given number of rounds
	static long num_rounds = 1000000000000;

	// These arrays only need to be as big as the number of rocks times the 
	// size of the wind array.  This should be the worst cost runtime of finding
	// the repeating pattern
	//
	// Array of history
	struct Rock* history = (struct Rock*)malloc(sizeof(struct Rock)*wind.size*5);
	// Array of rounds
	struct Round* rounds = (struct Round*)malloc(sizeof(struct Round)*wind.size*5);
	// Initialize our rounds array to zeros
	for (int i=0; i<wind.size*5; i++) {
		rounds[i].round = 0;
		rounds[i].height = 0;
	}
	
	// Iterate until i is either way too big or we've found the result
	for (int i=0; i<INT_MAX && !result; i++) {
		// Calculate our array index key.  This is simply the current type of 
		// rock times the size of the wind array plus where we are in the wind array
		// Essentially this creates a unique round for every position and every rock type
		key = (i%5)*wind.size+curr;
		// Check if we've already calculated this key
		if (rounds[key].height) {
			// If we have, calculate our magic quotient
			quotient = (num_rounds - (long)i) / (long)(i-rounds[key].round);
			// Calculate the remainder 
			remainder = (num_rounds - (long)i) % (long)(i-rounds[key].round);
			// If the remainder is zero that means we've found the correct multiplier
			if (remainder == 0L) {
				// Temporarily store our hieght in the result
				result = highest(history, i);
				// Calculate the actual result which is just the current height
				// plus the delta between the current height and the height of 
				// the stored round times the magic quotient
				//
				// Essentially we're multiplying the height of the pattern
				// times the number of rounds still needed to get to the number 
				// of roudns required
				result = result+(result-(long)rounds[key].height)*quotient;
			}
		}
		else {
			// Otherwise store the current round at the key
			rounds[key].height = highest(history, i);
			rounds[key].round = i;
		}
		// Drop another rock always until we've found the right pattern height
		curr = droprock(history, i, wind, curr);
	}

	// Free our arrays
	free(history);
	free(rounds);
	// Return the result
	return result;
}

// Function to parse the input file and return
// a string representing the wind
struct String processinput(FILE *in_file) {
	// stack vars
	struct String output = {.size = 0};
	char tmp;
	int curr = 0;

	// Get length of wind array
	while((tmp = fgetc(in_file)) && !feof(in_file)) {
		if (tmp == RIGHT || tmp == LEFT) {
			output.size++;
		}
	}
	// Go back to beginning of the file
	rewind(in_file);
	// Allocate our memory
	output.array = (char*)malloc(sizeof(char)*output.size);

	// Go over the file one character at a time
	while((tmp = fgetc(in_file)) && !feof(in_file)) {
		// If the current character is a direction
		// store it
		if (tmp == RIGHT || tmp == LEFT) {
			output.array[curr] = tmp;
			curr++;
		}
	}
	// Replace the ending newline with a terminater
	output.array[curr] = 0;

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
	struct String wind = processinput(in_file);
	// Solve part 1
	printf("The height of the rocks after 2022 rounds is %d\n", part1(wind));
	// Solve part 2
	printf("The height of the rocks after 1000000000000 rounds is %ld\n", part2(wind));
	// Clean up memory
	free(wind.array);
	// Close our file
	fclose(in_file);

	return EXIT_SUCCESS;
}
