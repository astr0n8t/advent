#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Create our "stack" datatype
struct stack {
	int height;
	int max_height;
	char* crate;
};

// Create our pop function
char pop(struct stack* curr) {
	char output = 0;
	if (curr->height) {
		output = curr->crate[curr->height-1];
		curr->crate[curr->height-1] = 0;
		curr->height = curr->height - 1;
	}
	return output;
}
// Create our push function
int push(struct stack* curr, char new) {
	if (curr->height == curr->max_height) {
		return EXIT_FAILURE;
	}
	curr->crate[curr->height] = new;
	curr->height = curr->height + 1;
	return EXIT_SUCCESS;
}
		
int main(int argc, char *argv[])
{
	// Read in our input file
	FILE *in_file = fopen("input.txt", "r");
	if (in_file == NULL) {
		printf("Could not open input file!");
		exit(-1);
	}
	// Declare our stack vars
	struct stack* crates_1;
	struct stack* crates_2;
	char buffer[100];
	char marker[100];
	// For part two we need a buffer to keep our chars in
	char* crate_2_buffer;
	int line_length = 0;
	int init_crates = 0;
	int num_crate_in_line = 1;
	int max_num_crate = 0;
	int max_height_stack = 0;
	int num_to_move = 0;
	int move_from = 0;
	int move_to = 0;

	// Iterate over the file a line at a time
	while(fgets(buffer, sizeof(buffer), in_file) != NULL && num_crate_in_line) {
		num_crate_in_line = 0;
		// Get the number of crates per line
		for (int i=0; i<strlen(buffer); i++) {
			if (buffer[i] == '[') {
				num_crate_in_line = num_crate_in_line + 1;
			}
		}
		// Increment our variables
		if (num_crate_in_line) {
			max_num_crate = num_crate_in_line;
			max_height_stack = max_height_stack + 1;
		}
		else {
			// If we're done, copy the last line into marker
			// since this contains our width definitions
			strcpy(marker, buffer);
		}
	}
	// Debug output
	printf("Max num crate: %d Max height stack: %d\n", max_num_crate, max_height_stack*max_num_crate);
	// Initialize our crates
	crates_1 = (struct stack*)malloc(max_num_crate*sizeof(struct stack));
	crates_2 = (struct stack*)malloc(max_num_crate*sizeof(struct stack));
	for (int i=0; i<max_num_crate; i=i+1) {
		crates_1[i].max_height = max_height_stack*max_num_crate;
		crates_1[i].height = 0;
		crates_1[i].crate = (char*)malloc(crates_1[i].max_height*sizeof(char));
		crates_2[i].max_height = max_height_stack*max_num_crate;
		crates_2[i].height = 0;
		crates_2[i].crate = (char*)malloc(crates_2[i].max_height*sizeof(char));
	}
	crate_2_buffer = (char*)malloc(crates_2[0].max_height*sizeof(char));

	// Start moving back up the file through the crates
	line_length = strlen(marker);
	fseek(in_file, -1, SEEK_CUR);
	fseek(in_file, -2*line_length, SEEK_CUR);

	// Actually initialize our crates
	for (int i=0; i<max_height_stack; i=i+1) {
		// Go one line at a time
		if(fgets(buffer, sizeof(buffer), in_file) != NULL) {
			// For each line iterate a char at a time
			for (int j=0; j<line_length-1; j=j+1) {
				// Check if the marker exists at this spot
				// and also check if a crate exists
				if (marker[j] != 0x20 && buffer[j] != 0x20) {
					// If it does, the crate is one less than the marker
					// and the value is simply at the current index
					push(&crates_1[marker[j] - '0' - 1], buffer[j]);
					push(&crates_2[marker[j] - '0' - 1], buffer[j]);
				}
			}
			// Move up two lines
			fseek(in_file, -2*line_length, SEEK_CUR);
		}
	}

	// Seek back to the end of the crates
	fseek(in_file, (line_length*max_height_stack)+1, SEEK_CUR);

	// Grab each instructioon
	while(fscanf(in_file, "move %d from %d to %d\n", &num_to_move, &move_from, &move_to) == 3) {
		// Iterate over the number of moves
		for (int i=0; i<num_to_move; i=i+1) {
			// For part one this is simple, for every move, simply pop from
			// one crate and push to the other
			push(&crates_1[move_to-1], pop(&crates_1[move_from-1]));
			// For part two, its a little more complex, we need to pop
			// and store these in a buffer for the time being
			crate_2_buffer[i] = pop(&crates_2[move_from-1]);
		}
		// For part two we need to iterate back over our moves
		for (int i=0; i<num_to_move; i=i+1) {
			// And for each push the crate values but backwards
			push(&crates_2[move_to-1], crate_2_buffer[num_to_move-i-1]);
		}
	}

	// Print out the tops of each stack
	printf("Top of each stack one: ");
	for (int i=0; i<max_num_crate; i=i+1) {
		printf("%c", pop(&crates_1[i]));
	}
	printf("\n");
	printf("Top of each stack two: ");
	for (int i=0; i<max_num_crate; i=i+1) {
		printf("%c", pop(&crates_2[i]));
	}
	printf("\n");

	// Free our memory
	free(crate_2_buffer);
	for (int i=0; i<max_num_crate; i=i+1) {
		free(crates_1[i].crate);
		free(crates_2[i].crate);
	}
	free(crates_1);
	free(crates_2);

	fclose(in_file);
	return EXIT_SUCCESS;
}
