#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct schedule {
	int start1;
	int stop1;
	int start2;
	int stop2;
};

// Function to return max of two ints
int max(int a, int b) {
	return (a >= b) ? a : b; 
}
// Function to return min of two ints
int min(int a, int b) {
	return (a <= b) ? a : b; 
}

int main(int argc, char *argv[])
{
	// Read in our file
	FILE *in_file = fopen("input.txt", "r");
	if (in_file == NULL) {
		printf("Could not open input file\n");
		return EXIT_FAILURE;
	}

	// Declare stack vars
	struct schedule current;
	int contained_total = 0;
	int overlapping_total = 0;
	
	// Iterate over the file, grabbing the pairs
	while(fscanf(in_file, "%d-%d,%d-%d\n", &current.start1, &current.stop1, 
				&current.start2, &current.stop2) == 4) {

		// Check for contained ranges
		if ((min(current.start1, current.start2) == current.start1 
			&& max(current.stop1, current.stop2) == current.stop1) ||
			(min(current.start2, current.start1) == current.start2 
			&& max(current.stop2, current.stop1) == current.stop2)) {

			contained_total = contained_total + 1;
		}

		// Check for overlapping ranges
		if ((max(current.stop1, current.start2) == current.stop1
			&& min(current.start1, current.start2) == current.start1) ||
			(max(current.stop2, current.start1) == current.stop2
			&& min(current.start2, current.start1) == current.start2)) {

			overlapping_total = overlapping_total + 1;
		}
	}

	// Output the total
	printf("The total number contained in is %d\n", contained_total);
	printf("The total number overlapping is %d\n", overlapping_total);

	fclose(in_file);
	return EXIT_SUCCESS;
}
