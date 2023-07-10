#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function clears the crt screen
void clear(char crt[][40]) {
	for (int i=0; i<6; i=i+1) {
		for (int j=0; j<40; j=j+1) {
			crt[i][j] = '.';
		}
	}
	return;
}

// Function prints out the crt screen
void print(char crt[][40]) {
	for (int i=0; i<6; i=i+1) {
		for (int j=0; j<40; j=j+1) {
			printf("%c", crt[i][j]);
		}
		printf("\n");
	}
	return;
}


// This function decides what should be on the 
// crt per 240 cycle draw
void draw(int pos, int cycle, char crt[][40]) {
	// Figure out what the current index is
	int y = (cycle-1) / 40;
	int x = (cycle-1) % 40;

	// Check if our current spot is within the
	// sprite to draw
	if (x == pos-1 || x == pos ||  x == pos+1) {
		crt[y][x] = '#';
	}

	// If we've completed a full clock cycle
	// print the crt and clear it
	if (cycle % 240 == 0) {
		print(crt);
		clear(crt);
	}
	return;
}
		
// The clock function is ran once per cycle
int clock(int reg, int* cycle, int strength, char crt[][40]) {
	// Declare our specific checkpoints
	static int checkpoints[6] = {20, 60, 100, 140, 180, 220};
	// Increment the cycle
	*cycle = *cycle + 1;
	// Update our CRT
	draw(reg, *cycle, crt);
	// Check if we need to re-compute signal strength
	for (int i=0; i<6; i=i+1) {
		if (*cycle == checkpoints[i]) {
			strength = strength + reg*(*cycle);
		}
	}
	// Return the signal strength
	return strength;
}

// addx operation
int addx(int value, int* reg, int* cycle, int strength, char crt[][40]) {
	// Run the clock twice
	strength = clock(*reg, cycle, strength, crt);
	strength = clock(*reg, cycle, strength, crt);
	// Perform the addx operation
	*reg = *reg + value;
	// Return the signal strength
	return strength;
}

// noop operation
int noop(int reg, int* cycle, int strength, char crt[][40]) {
	// Run the clock once
	strength = clock(reg, cycle, strength, crt);
	// Return the signal strength
	return strength;
}

int main(int argc, char *argv[])
{
	// Read in our file
	FILE *in_file = fopen("input.txt", "r");
	if (in_file == NULL) {
		printf("Could not open input file\n");
		return EXIT_FAILURE;
	}
	// stack vars
	// X reg
	int x = 1;
	int cycle = 0;
	int num_args;
	char op[5];
	int arg = 0;
	int strength = 0;
	char crt[6][40];

	// Clear our crt before we begin
	clear(crt);

	// Run through each opcode in the file
	while((num_args = fscanf(in_file, "%s %d\n", op, &arg)) && num_args > 0) {
		// If there is only one argument
		// we know its a noop
		if (num_args == 1) {
			strength = noop(x, &cycle, strength, crt);
		}
		// Otherwise it should be two in
		// which case its a addx
		else {
			strength = addx(arg, &x, &cycle, strength, crt);
		}
	}

	// Output the signal strength
	printf("The sum of signal strengths for part 1 would be %d\n", strength);

	fclose(in_file);
	return EXIT_SUCCESS;
}
