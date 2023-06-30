#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	// Read in our input file
	FILE *in_file = fopen("input.txt", "r");
	if (in_file == NULL) {
		printf("Could not open input file!");
		exit(-1);
	}
	// Declare our stack vars
	int current_val = 0;
	int current_elf = 0;
	int highest_elf[3] = {0, 0, 0};
	int temp = 0;
	char buffer[100];

	// Iterate over the file a line at a time
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		// Check if the line is blank
		if (buffer[0] != '\n') {
			// See if we can get the int out of the line
			if (sscanf(buffer, "%d", &current_val) == 1) {
				// Add to the current elf's total
				current_elf = current_elf + current_val;
			}
		}
		else {
			// Otherwise we assume we hit a double newline
			// CHeck if our elf has more calories than the current highest
			for (int i=0; i<3; i=i+1) {
				if (current_elf > highest_elf[i]) {
					temp = highest_elf[i];
					// If it does set our elf as the highest elf
					highest_elf[i] = current_elf;
					current_elf = temp;
				}
			}
			// Reset the current elf
			current_elf = 0;
		}
			
	}

	printf("The highest amount of calories being caried is %d\n", highest_elf[0]);
	temp = highest_elf[0] + highest_elf[1] + highest_elf[2];
	printf("The total amount of calories being carried by the top three elfs is %d\n", temp);

	fclose(in_file);
	return EXIT_SUCCESS;
}
