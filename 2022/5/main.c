#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct stack {
	int height;
	int max_height;
	char* crate;
};

char pop(struct stack* curr) {
	char output = 0;
	if (curr->height) {
		output = curr->crate[curr->height-1];
		curr->crate[curr->height-1] = 0;
		curr->height = curr->height - 1;
	}
	return output;
}
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
	struct stack* crates;
	char buffer[100];
	char marker[100];
	int line_length = 0;
	int init_crates = 0;
	int num_crate_in_line = 1;
	int max_num_crate = 0;
	int max_height_stack = 0;

	// Iterate over the file a line at a time
	while(fgets(buffer, sizeof(buffer), in_file) != NULL && num_crate_in_line) {
		num_crate_in_line = 0;
		for (int i=0; i<strlen(buffer); i++) {
			if (buffer[i] == '[') {
				num_crate_in_line = num_crate_in_line + 1;
			}
		}
		if (num_crate_in_line) {
			max_num_crate = num_crate_in_line;
			max_height_stack = max_height_stack + 1;
		}
		else {
			strcpy(marker, buffer);
		}
	}
	printf("Max num crate: %d Max height stack: %d\n", max_num_crate, max_height_stack);
	// Initialize our crates
	crates = (struct stack*)malloc(max_num_crate*sizeof(struct stack));
	for (int i=0; i<max_num_crate; i=i+1) {
		crates[i].max_height = max_height_stack;
		crates[i].height = 0;
		crates[i].crate = (char*)malloc(max_height_stack*sizeof(char));
	}
	// Start moving back up the file through the crates
	line_length = strlen(marker);
	fseek(in_file, -1, SEEK_CUR);
	fseek(in_file, -2*line_length, SEEK_CUR);

	for (int i=0; i<max_height_stack; i=i+1) {
		if(fgets(buffer, sizeof(buffer), in_file) != NULL) {
			for (int j=0; j<line_length-1; j=j+1) {
				if (marker[j] != 0x20 && buffer[j] != 0x20) {
					push(&crates[marker[j] - '0' - 1], buffer[j]);
				}
			}

			fseek(in_file, -2*line_length, SEEK_CUR);
		}
	}

	for(int i=0; i<max_num_crate; i=i+1) {
		printf("Value in crate %d is %c\n", i, pop(&crates[i]));
	}

	// Free our memory
	for (int i=0; i<max_num_crate; i=i+1) {
		free(crates[i].crate);
	}
	free(crates);

	fclose(in_file);
	return EXIT_SUCCESS;
}
