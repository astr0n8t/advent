#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// Use a bitfield to keep track of numbers we've seen
// we use a ulong since it contains 64 bits and we need like 56
// We also need to use UL to denote size of the bitshift
ulong getbit(ulong bitfield, char character) {
	return bitfield & (1UL << character - 'A');
}

ulong setbit(ulong bitfield, char character) {
	return bitfield | (1UL << character - 'A');
}

int main(int argc, char *argv[])
{
	// Read in our file and fail if null
	FILE *in_file = fopen("input.txt", "r");
	if (in_file == NULL) {
		printf("Could not open input file\n");
		return EXIT_FAILURE;
	}
	// Declare our stack vars
	char curr;
	int index = 0;
	int marker_found = 0;
	int unique = 1;
	ulong bitfield = 0;
	// Indicate length of file marker here
	// 4 for first part, 14 for second
	char buff[14];
	int marker_length = 14;

	// Iterate over our file one char at a time until we have our marker
	while(fscanf(in_file, "%c", &curr) == 1 && !marker_found) {
		// Skip newlines
		if (curr == '\n') {
			continue;
		}
		// Reset bitfield and unique vars
		bitfield = 0;
		unique = 1;
		// Shift our buffer back by one
		for (int i=0; i<marker_length-1; i=i+1) {
			buff[i] = buff[i+1];
		}
		// Set our current char to the last one in the buffer
		buff[marker_length-1] = curr;
		// Check if we have enough characters for a start of file marker
		if (index > marker_length-1) {
			// Iterate over the buffer until a duplicate is found
			for (int i=0; i<marker_length && unique; i=i+1) {
				// Check if the char is a duplicate 
				if (!getbit(bitfield, buff[i])) {
					// Record that we saw the char
					bitfield = setbit(bitfield, buff[i]);
				}
				else {
					// Otherwise set unique to false because this
					// is not our marker
					unique = 0;
				}
			}
			// If unique is still true
			// then this is our marker
			if (unique) {
				marker_found = unique;
			}
		}
		// Increment our index
		index = index + 1;
	}

	// Output the result
	printf("The num after the start of file marker is %d\n", index);

	fclose(in_file);
	return EXIT_SUCCESS;
}
