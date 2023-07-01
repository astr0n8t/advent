#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct schedule {
	int start1;
	int stop1;
	int start2;
	int stop2;
};

int max(int a, int b) {
	return (a >= b) ? a : b; 
}
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
	int total = 0;
	
	//
	// Iterate over the file, grabbing the pairs
	while(fscanf(in_file, "%d-%d,%d-%d\n", &current.start1, &current.stop1, 
				&current.start2, &current.stop2) == 4) {
		if ((min(current.start1, current.start2) == current.start1 
			&& max(current.stop1, current.stop2) == current.stop1) ||
			(min(current.start2, current.start1) == current.start2 
			&& max(current.stop2, current.stop1) == current.stop2)) {
			printf("%d-%d,%d-%d\n", current.start1, current.stop1, 
				current.start2, current.stop2);
			total = total + 1;
		}
	}

	// Output the total
	printf("The total number contained in is %d\n", total);

	fclose(in_file);
	return EXIT_SUCCESS;
}
