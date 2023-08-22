#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>

// ELF constant
#define ELF '#'

// Min/max array indexes
#define MIN_X 0
#define MAX_X 1
#define MIN_Y 2
#define MAX_Y 3

// Complex directions
#define NORTH 1*I
#define SOUTH -1*I
#define WEST -1
#define EAST 1

// Constant to represent undefined location
// must not be valid location
#define UNDEFINED 999999

// An elf is simply a current location
// and the next location
typedef struct elf {
	double complex loc;
	double complex next;
} Elf;

// An elflist is a list of elves
typedef struct elflist {
	Elf* array;
	size_t size;
	size_t max;
} ElfList;

// Really inefficient way to see if a spot is empty
int empty(ElfList elves, double complex loc) {
	// Simply iterate over the array
	for (int i=0; i<elves.size; i++) {
		// Check if the current elf's location is the given location
		if (elves.array[i].loc == loc) {
			// If it is return false
			return 0;
		}
	}
	// Otherwise return true
	return 1;
}

// Really inefficient way to see if the elf
// at the given index has chosen a unique next
// location
int unique(ElfList elves, int id) {
	// Simply iterate over the array
	for (int i=0; i<elves.size; i++) {
		// Check if there is another elf with the same next location
		if (i != id && elves.array[i].next == elves.array[id].next) {
			// If there is return false
			return 0;
		}
	}
	// Otherwise return true
	return 1;
}

// Solves for one round of the simulation
// given a specific offset
int solve(ElfList* elves, int offset) {
	// stack vars
	// Our directions to check
	static double complex directions[4][3] = {
		{NORTH, NORTH + EAST, NORTH + WEST},
		{SOUTH, SOUTH + EAST, SOUTH + WEST},
		{WEST,  NORTH + WEST, SOUTH + WEST},
		{EAST,  NORTH + EAST, SOUTH + EAST}};
	// Our directions for around a specific elf
	static double complex around[8] = {
		NORTH, 
		NORTH+EAST,
		EAST,
		SOUTH+EAST,
		SOUTH,
		SOUTH+WEST,
		WEST,
		NORTH+WEST
	};
	int move = 0;
	int next_dir = 0;

	// Iterate over all the elves
	for (int i=0; i<elves->size; i++) {
		// Set the next move to undefined
		elves->array[i].next = UNDEFINED;
		// Reset whether the elf should move
		move = 0;
		// Iterate around the elf until another elf is found
		for (int j=0; j<8 && !move; j++) {
			// Check for another elf next to the current elf
			if (!empty(*elves, elves->array[i].loc+around[j])) {
				// If there is an elf we need to move
				move = 1;
			}
		}
		// Check if we need to move
		if (move) {
			// Try every direction or until we find a valid move
			for (int k=0; k<4 && elves->array[i].next == UNDEFINED; k++) {
				// Calculate the next direction to try given our offset
				next_dir = (offset+k) % 4;
				// Check if the direction has empty space
				if (empty(*elves, elves->array[i].loc+directions[next_dir][0]) &&
					empty(*elves, elves->array[i].loc+directions[next_dir][1]) &&
					empty(*elves, elves->array[i].loc+directions[next_dir][2])) {
					// Set our next location to that direction plus our current location
					elves->array[i].next = elves->array[i].loc+directions[next_dir][0];
				}
			}
		}
	}

	// Rest the move calculation
	move = 0;
	// Iterate over the array again
	for (int i=0; i<elves->size; i++) {
		// If there is a next location and its unique
		if (elves->array[i].next != UNDEFINED && unique(*elves, i)) {
			// Set our current location to the next location
			elves->array[i].loc = elves->array[i].next;
			// Increment move to show that we moved
			move++;
		}
	}

	// Return the number of moves 
	// so it will be 0 if no elves moved
	// this round
	return move;
}

// Solve part1
int part1(ElfList elves) {
	// Stack vars
	double dimensions[4] = {0, 0, 0, 0};
	double x = 0;
	double y = 0;
	// Run ten rounds
	for (int i=0; i<10; i++) {
		solve(&elves, i);
	}
	// Iterate over the entire array
	for (int i=0; i<elves.size; i++) {
		// Get the real coordinates
		x = creal(elves.array[i].loc);
		y = cimag(elves.array[i].loc);
		// Check if these are our mins or maxes
		dimensions[MIN_X] = (x < dimensions[MIN_X]) ? x : dimensions[MIN_X];
		dimensions[MAX_X] = (x > dimensions[MAX_X]) ? x : dimensions[MAX_X];
		dimensions[MIN_Y] = (y < dimensions[MIN_Y]) ? y : dimensions[MIN_Y];
		dimensions[MAX_Y] = (y > dimensions[MAX_Y]) ? y : dimensions[MAX_Y];
	}

	// Return the area of the mins and maxes without our elves
	return (dimensions[MAX_X] - dimensions[MIN_X] + 1) * 
		(dimensions[MAX_Y] - dimensions[MIN_Y] + 1) - elves.size;
}

// solve part2
int part2(ElfList elves) {
	// Stack vars
	// start at 10 since we ran 10 rounds in part1
	int answer = 10;
	// Continue to solve until elves cannot move
	while (solve(&elves, answer)) {
		answer++;
	}
	// Return the round plus 1
	return ++answer;
}


// Processes the selected input file and stores the result in the 
// given ElfList
void processinput(char* filename, ElfList* output) {
	// Read in our file
	FILE *in_file = fopen(filename, "r");
	if (in_file == NULL) {
		printf("Could not open input file\n");
		exit(EXIT_FAILURE);
	}

	// stack vars
	char buffer[100];
	int height = 0;
	// Iterate over the file line by line
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		// Check if the line is blank
		if (buffer[0] != '\n') {
			// Iterate over the line
			for (int i=0; i<strlen(buffer); i++) {
				// Check for elfs in the line
				if (buffer[i] == ELF) {
					// Increment max for each elf
					output->max++;
				}
			}
			// Also increment our height
			height++;
		}
	}
	// Go back to the beginning of the file
	rewind(in_file);
	// Allocate our array for the elfs
	output->array = malloc((sizeof *output->array)*output->max);

	// Go back over the file line by line
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		// Check if the line is blank
		if (buffer[0] != '\n') {
			// Decrement our height cause we want to add coordinates
			// from the top down
			height--;
			// Go over the line character by character
			for (int i=0; i<strlen(buffer)-1; i++) {
				// Check if we're looking at an elf
				if (buffer[i] == ELF) {
					// If its an elf, calculate its complex coordinates and
					// add it to our array
					output->array[output->size++].loc = i + height*I;
				}
			}
		}
	}

	// Close our file in case we want to call this function again
	fclose(in_file);
	return;
}

int main(int argc, char *argv[])
{
	// Stack vars
	ElfList elves = {.array=NULL, .size=0, .max=0};
	// Get our input
	processinput("input.txt", &elves);
	// Solve part1
	printf("The number of empty ground tiles after 10 rounds is %d\n", part1(elves));
	// Solve part2
	printf("The first round where no elf moved was %d\n", part2(elves));

	// Free our allocated array
	free(elves.array);
	return EXIT_SUCCESS;
}
