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

// This function converts characters
// into the priority
int getpriority(char character) {
	int priority = 0;
	if (character >= 'a') {
		priority = character - 'a' + 1;
	}
	else {
		priority = character - 'A' + 27;
	}
	return priority;
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
	char duplicate;
	int sum_priority = 0;
	int sum_priority_group = 0;
	char buffer[100];
	int line_length;
	int mid_point;
	ulong bitfield = 0;
	ulong bitfield3[3] = {0, 0, 0};
	ulong badge = 0;
	int group = 0;

	// Iterate over our file one line at a time
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		// Check if the line is blank
		if (buffer[0] == '\n') {
			continue;
		}

		// Reset our variables
		duplicate = '0';
		bitfield = 0;
		line_length = strlen(buffer);
		mid_point = line_length / 2;

		// For the first half, initialize the main bitfield
		// as well as the specific group bitfield
		for (int i=0; i<mid_point; i=i+1) {
			bitfield = setbit(bitfield, buffer[i]);
			bitfield3[group] = setbit(bitfield3[group], buffer[i]);
		}
		// Next check if any character in the second half is in the main
		// bitfield while also finishing to set the specific group bitfield
		for (int i=mid_point; i<line_length-1; i=i+1) {
			bitfield3[group] = setbit(bitfield3[group], buffer[i]);
			// If the character is in the main bitfield
			// it is our duplicate in the bucket
			if (getbit(bitfield, buffer[i])) {
				duplicate = buffer[i];
			}
		}

		// Add to the main priority
		sum_priority = sum_priority + getpriority(duplicate);
		// Increase our group count
		group = group + 1;
		// If we just finished a third run, evaluate the group array
		if (group % 3 == 0) {
			// Find the single character that exists among all
			// three bitfields
			badge = bitfield3[0] & bitfield3[1] & bitfield3[2];
			// Reset our character variable
			duplicate = '0';

			// Iterate through the bitfield till we find the
			// single turned on bit
			for (int i=0; i<64 && duplicate == '0'; i=i+1) {
				// Check if the bit is on
				if (badge & (1UL << i)) {
					// Convert that bit to a character
					duplicate = (char) (i + 'A');
				}
			}

			// Add to our group priority
			sum_priority_group = sum_priority_group + getpriority(duplicate);

			// Reset our variables for another run
			bitfield3[0] = 0;
			bitfield3[1] = 0;
			bitfield3[2] = 0;
			group = 0;
		}
	}

	printf("The sum of priorities is %d\n", sum_priority);
	printf("The sum of group priorities is %d\n", sum_priority_group);

	fclose(in_file);
	return EXIT_SUCCESS;
}
