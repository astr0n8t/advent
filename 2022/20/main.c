#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// We're going to keep track of the item both in 
// the current list and the overall list
// for simplicity
typedef struct item {
	long val;
	int pos;
} Item;

// Define a list struct
typedef struct list {
	Item* array;
	int size;
} List;


// Deep copy a list
List deepcopy(List src) {
	// Create a copy of the stack data
	List dst = src;
	// Allocate new space for our copy
	dst.array = malloc((sizeof *dst.array)*dst.size);
	// Copy our heap memory
	memcpy(dst.array, src.array, (sizeof *dst.array)*dst.size);
	// Return the list
	return dst;
}


// Mixes the list with the multiplier for the number of rounds
List mix(List src, int mul, int num) {

	// Set our multiplier and reset poositions
	for (int i=0; i<src.size; i++) {
		src.array[i].val *= mul;
		src.array[i].pos = i;
	}

	// Stack vars
	// Create a deepcopy of our list
	List working = deepcopy(src);
	int curr = 0;
	long index = 0;
	long nindex = 0;
	long move = 0;
	Item tmp;

	// Main mix loop for this number of rounds
	for (int i=0; i<working.size*num; i++) {
		// Computer the src array index
		curr = i % working.size;
		// Retrieve the index in the woorking copy
		index = src.array[curr].pos;
		// Get the value for the number of shifts
		move = src.array[curr].val;
		// Compute the candidate new index for the value
		// which is our number of moves mod our list size without the current value
		// added to the current index
		nindex = index + (move % (working.size-1));
		
		// If the number was positive but larger or equal to the size
		if (move > 0 && nindex >= (working.size-1)) {
			// Remove the size of the list
			nindex -= working.size-1;
		}
		// If the number was negative and larger or equal to the size so its
		// also negative
		else if (move < 0 && nindex <= 0) {
			// Add the size of the list
			nindex += working.size-1;
		}

		// Store a temporary copy of the value we want to move
		tmp = working.array[index];
		// Check if we're moving forwards through the array
		if (nindex > index) {
			// Shift the array back
			for (int j=index; j<nindex; j++) {
				// Set current index to the next item
				working.array[j] = working.array[j+1];
				// Store the position back in our source array
				src.array[working.array[j].pos].pos = j;
			}
		}
		else {
			// Otherwise we're moving backwards through the array
			// so shift the array forward
			for (int j=index; j>nindex; j--) {
				// Set the current index to the previous item
				working.array[j] = working.array[j-1];
				// Store the position back in our source array
				src.array[working.array[j].pos].pos = j;
			}
		}
		// Store our value into the target index
		working.array[nindex] = tmp;
		// Update the current items index in the source array
		src.array[working.array[nindex].pos].pos = nindex;

	}

	// Return the working list
	return working;
}

// Solve part1
int part1(List puzzle) {
	// stack vars
	int sum = 0;
	int zeroth_index = 0;
	// Mix our input
	List mixed = mix(puzzle, 1, 1);

	// Find the index of the value zero in the array
	for (int i=0; i<mixed.size; i++) {
		if (mixed.array[i].val == 0) {
			zeroth_index = i;
			break;
		}
	}

	// Compute the sum
	sum += mixed.array[(1000+zeroth_index)%mixed.size].val;
	sum += mixed.array[(2000+zeroth_index)%mixed.size].val;
	sum += mixed.array[(3000+zeroth_index)%mixed.size].val;

	// Free our memory from the deepcopy
	free(mixed.array);

	// Rtuern the sum
	return sum;
}

// Solve part2
long part2(List puzzle) {
	// Stack vars
	long sum = 0;
	int zeroth_index = 0;
	// Mix the input with the multiplier and number of rounds
	List mixed = mix(puzzle, 811589153, 10);

	// Find the index of the value zero in the array
	for (int i=0; i<mixed.size; i++) {
		if (mixed.array[i].val == 0) {
			zeroth_index = i;
			break;
		}
	}

	// Compute the sum
	sum += mixed.array[(1000+zeroth_index)%mixed.size].val;
	sum += mixed.array[(2000+zeroth_index)%mixed.size].val;
	sum += mixed.array[(3000+zeroth_index)%mixed.size].val;

	// Free our memory from the deepcopy
	free(mixed.array);

	// Return the sum
	return sum;
}

// Process the specified input file
List processinput(char* filename) {
	// Read in our input file
	FILE *in_file = fopen(filename, "r");
	if (in_file == NULL) {
		printf("Could not open input file!");
		exit(-1);
	}

	// Stack vars
	List output = {.size=0};
	char buffer[100];
	int curr = 0;
	// Get size of list
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		// Check if the line is blank
		if (buffer[0] != '\n') {
			output.size++;
		}
	}
	// Go back to beginning of the file
	rewind(in_file);

	// Allocate our memory
	output.array = malloc((sizeof *output.array)*output.size);

	// Iterate over every value
	while(curr < output.size && fscanf(in_file, "%ld\n", &output.array[curr].val) == 1) {
		// Add the value to the array and set the position
		output.array[curr].pos = curr;
		curr++;
	}

	// Close the file so we can call this function again
	fclose(in_file);

	// Return the generated list
	return output;
}

int main(int argc, char *argv[])
{
	// Process input file
	List puzzle = processinput("input.txt");
	// Solve part1
	printf("Answer part 1: %d\n", part1(puzzle));
	// Solve part2
	printf("Answer part 2: %ld\n", part2(puzzle));
	// Free our memory
	free(puzzle.array);
	return EXIT_SUCCESS;
}
