#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <complex.h>

// Define complex directions
#define UP 1*I
#define DOWN -1*I
#define LEFT -1
#define RIGHT 1

// Define wind and clear markers
#define WIND 'w'
#define CLEAR '.'

// Wind is a position with a directional step
typedef struct wind {
	double complex position;
	double complex step;
} Wind;

// A list of wind also contains height and width of the map
typedef struct windlist {
	Wind* array;
	size_t size;
	size_t max;
	size_t height;
	size_t width;
} WindList;

// A state is a position and a score
typedef struct state {
	double complex position;
	int score;
} State;

// A list of states 
typedef struct statelist {
	State* array;
	size_t size;
	size_t max;
} StateList;

// Comparison function to pass to qsort
int cmpstate(const void* av, const void* bv) {
	// Takes in pointers to states
	State* a = (State*)av;
	State* b = (State*)bv;
	// We want the one with the best score
	if (a->score < b->score) {
		return 1;
	}
	return 0;
}

// Sorts b into a
void sortinto(StateList* a, StateList* b) {
	// Sort B
	qsort(b->array, b->size, sizeof(State), cmpstate);

	// Remove duplicate positions from b
	// since we sorted on the best scores, we should
	// only be removing duplicates with worst scores
	for (int i=1; i<b->size; i++) {
		// Check for a duplicate
		if (b->array[i].position == b->array[i-1].position) {
			// Decrement the size of b
			b->size--;
			// Shift b back by one
			for (int j=i; j<b->size; j++) {
				b->array[j] = b->array[j+1];
			}
			// re-do this position in the array
			i--;
		}
	}

	// Reset A
	a->size = 0;
	// Copy as much of B that we can fit into A
	for (int i=0; i<a->max && i<b->size; i++) {
		a->array[a->size++] = b->array[i];
	}

	return;
}

// Inserts a state into a statelist
void stateinsert(StateList* state, State current) {
	// Check if our statelist is at the max size
	if (state->size == state->max) {
		// Add 1000 entries to the statelist
		state->max += 1000;
		// Re-allocate our memory with the new size
		state->array = realloc(state->array, (sizeof *state->array)*(state->max));
	}
	// Insert the state into the statelist
	state->array[state->size++] = current;

	return;
}

int reached(StateList states, double complex end) {
	for (int i=0; i<states.size; i++) {
		if (states.array[i].position == end) {
			return 1;
		}
	}
	return 0;
}

// Constructs the wind map from a wind list
void construct(char** map, size_t h, size_t w, WindList wind) {
	// Stack vars
	int x = 0;
	int y = 0;

	// Set the map to clear initially
	for (int i=0; i<h; i++) {
		for (int j=0; j<w; j++) {
			map[i][j] = CLEAR;
		}
	}

	// Iterate over the wind list
	for (int i=0; i<wind.size; i++) {
		// Calculate the coordinates modulo the array bounds
		x = (int)(w+creal(wind.array[i].position)) % w;
		y = (int)(h+cimag(wind.array[i].position)) % h;
		// Update the position in the array to the new corrected position 
		wind.array[i].position = x + y*I;
		// Set the position in the map to wind
		map[y][x] = WIND;
	}

	return;
}

// Runs one cycle of the wind
void cycle(WindList wind) {
	// Iterate over the wind array
	for (int i=0; i<wind.size; i++) {
		// Add a step to each position
		wind.array[i].position += wind.array[i].step;
	}
	return;
}

// Solution function for navigating from start to end in a specific direction
int solve(WindList wind, int num_states, double complex start, double complex end, double complex tdir) {
	// Stack vars
	int min = 0;
	int x = 0;
	int y = 0;
	State current = {.position = start, .score = 0};
	State current_test;
	StateList states = {.size=0, .max=num_states};
	StateList new_states = {.size=0, .max=num_states};
	size_t h = wind.height;
	size_t w = wind.width;
	// Allocate space for our map of the wind
	char** map = malloc((sizeof *map)*h+(sizeof **map)*h*w);
	// Do some pointer math to setup our arrays properly
	for (int i=0;i<h; i++) {
		map[i] = (char *)((char *)(map+h)+w*i);
	}

	// Allocate our arrays
	states.array = malloc(sizeof(State)*states.max);
	new_states.array = malloc(sizeof(State)*new_states.max);

	// Set the first item in the primary statelist
	// The only item is a single ore robot
	states.array[0] = current;
	states.size = 1;

	// Iterate until one state has reached the end
	while (!reached(states, end)) {
		// Cycle the wind
		cycle(wind);
		// Construct our map from the wind
		construct(map, h, w, wind);

		// Reset our new_states list
		new_states.size = 0;
		// Iterate for every state in our states list
		for (int i=0; i<states.size; i++) {
			// Copy the array item into current
			current = states.array[i];

			// Calculate the x/y coords
			x = (int)creal(current.position);
			y = (int)cimag(current.position);
			// Check if we can stay here
			if (current.position == end || current.position == start || 
					(x >= 0 && y >= 0 && x < w && y < h && map[y][x] != WIND)) {
				// If we can insert the current position into the new states array
				stateinsert(&new_states, current);
			}

			// Test the up direction
			current_test.position = current.position+UP;
			current_test.score = current.score;
			x = (int)creal(current_test.position);
			y = (int)cimag(current_test.position);
			if (current_test.position == end || 
					(x >= 0 && y >= 0 && x < w && y < h && map[y][x] != WIND)) {
				// We weight vertical movements in the correct direction as 1
				// and in the wrong direction as -1
				if (tdir == DOWN) {
					current_test.score--;
				}
				else {
					current_test.score++;
				}
				stateinsert(&new_states, current_test);
			}
			// Test the down direction
			current_test.position = current.position+DOWN;
			current_test.score = current.score;
			x = (int)creal(current_test.position);
			y = (int)cimag(current_test.position);
			if (current_test.position == end || 
					(x >= 0 && y >= 0 && x < w && y < h && map[y][x] != WIND)) {
				// We weight vertical movements in the correct direction as 1
				// and in the wrong direction as -1
				if (tdir == UP) {
					current_test.score--;
				}
				else {
					current_test.score++;
				}
				stateinsert(&new_states, current_test);
			}
			// Test the left direction
			current_test.position = current.position+LEFT;
			current_test.score = current.score;
			x = (int)creal(current_test.position);
			y = (int)cimag(current_test.position);
			if (current_test.position == end || 
					(x >= 0 && y >= 0 && x < w && y < h && map[y][x] != WIND)) {
				// We weight horizontal movements in the correct direction as 2
				// and in the wrong direction as -2
				if (tdir == DOWN) {
					current_test.score -= 2;
				}
				else {
					current_test.score += 2;
				}
				stateinsert(&new_states, current_test);
			}
			// Test the right direction
			current_test.position = current.position+RIGHT;
			current_test.score = current.score;
			x = (int)creal(current_test.position);
			y = (int)cimag(current_test.position);
			if (current_test.position == end ||  
					(x >= 0 && y >= 0 && x < w && y < h && map[y][x] != WIND)) {
				// We weight horizontal movements in the correct direction as 2
				// and in the wrong direction as -2
				if (tdir == UP) {
					current_test.score -= 2;
				}
				else {
					current_test.score += 2;
				}
				stateinsert(&new_states, current_test);
			}

		}
		// Sort new_states into states
		// This effectively selects only the best states up to the number
		// specified by num_states
		sortinto(&states, &new_states);
		// Increment our number of rounds
		min++;
	}

	// Free our allocated memory
	free(new_states.array);
	free(states.array);
	free(map);

	// Return the minimum number of moves
	return min;
}


// Solve part1
int part1(WindList wind) {
	// Just return the distance from actual start to end
	double complex start = wind.height*I;
	double complex end = -1*I + wind.width-1;
	return solve(wind, wind.height*wind.width, start, end, DOWN);
}

// Solve part2
int part2(WindList wind, int answer) {
	double complex start = wind.height*I;
	double complex end = -1*I + wind.width-1;
	// Go from end to start and add it to our answer from part1
	answer += solve(wind, wind.height*wind.width, end, start, UP);
	// Go from start to end again and add it to our answer
	answer += solve(wind, wind.height*wind.width, start, end, DOWN);
	// Return the total answer of start -> end -> start -> end
	return answer;
}


// Process the specified input file
void processinput(char* filename, WindList* output) {
	// Read in our input file
	FILE *in_file = fopen(filename, "r");
	if (in_file == NULL) {
		printf("Could not open input file!");
		exit(-1);
	}

	// Stack vars
	char buffer[500];
	int j = 0;
	// Initialize our windlist
	*output = (WindList){.size=0, .max=0, .height=0, .width=0};
	
	// Ignore the first line
	fgets(buffer, sizeof(buffer), in_file);

	// Iterate over the file once to get dimensions
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		// Check if this is within the array
		if (buffer[0] != '\n' && buffer[1] != '#') {
			// Increment our hieght
			output->height++;
			// Set our width to the width minus the borders and newline
			output->width = strlen(buffer)-3;
			// Get the number of wind chars in this line
			for (int i=1; i<strlen(buffer)-2; i++) {
				if (buffer[i] != '.') {
					output->max++;
				}
			}
		}
	}
	// Go back to begining of the file
	rewind(in_file);
	// Ignore the first line
	fgets(buffer, sizeof(buffer), in_file);

	// Allocate our memory
	output->array = malloc((sizeof *output->array)*output->max);

	// Iterate over the entire file
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		// If the line is blank or a border just ignore it
		if (buffer[0] == '\n' || buffer[1] == '#') {
			continue;
		}

		// Go over each line character by character ignoring th borders
		for (int i=1; i<strlen(buffer)-2; i++) {
			// Check if the current char is a wind
			if (buffer[i] != '.') {
				// Calculate the position of the wind
				output->array[output->size].position = (i-1) + (output->height-j-1)*I;
				// Figure out the direction of the wind
				switch (buffer[i]) {
					case '>':
						output->array[output->size++].step = RIGHT;
						break;
					case '<':
						output->array[output->size++].step = LEFT;
						break;
					case '^':
						output->array[output->size++].step = UP;
						break;
					case 'v':
						output->array[output->size++].step = DOWN;
						break;
				}
			}
		}
		// Mark that we're done with this line
		j++;
	}

	// Close our file in case we want to call processinput again
	fclose(in_file);

	return;
}

int main(int argc, char *argv[])
{
	// Stack vars
	WindList wind;
	// Process our input file into wind
	processinput("input.txt", &wind);
	// Calculate the answer for part1
	int answer1 = part1(wind);
	// Solve part1
	printf("Minimum number of moves to navigate the maze: %d\n", answer1);
	// Solve part2 using answer from part 1
	printf("Minimum number of moves to navigate the maze: %d\n", part2(wind, answer1));

	// Free our allocated memory
	free(wind.array);
	return EXIT_SUCCESS;
}
