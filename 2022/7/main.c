#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct directory {
	char name[20];
	int size;
	int parent;
};


// Our function to handle the cd command
int cd(char name[20], struct directory* fs, int index, int max) {
	// check if we need to cd to the root
	if (strcmp(name, "/") == 0) {
		index = 0;
	}
	// Check if we need to go back a directory
	else if	(strcmp(name, "..") == 0) {
		index = fs[index].parent;
	}
	else {
		// We need the directory that has both the correct name and parent
		for (int i=0; i<max; i=i+1) {
			if (strcmp(name, fs[i].name) == 0 && index == fs[i].parent) {
				index = i;
				break;
			}
		}
	}
	//printf("cd to %s\n", fs[index].name);
	// The fallback case will always be the current directory
	return index;
}

// This function handles the ls command
void ls(struct directory* fs, int index, int max, FILE* in_file) {
	// We need quite a few stackvars here
	char buffer[100];
	char filename[20];
	int filesize = 0;
	int curr = 0;

	// Iterate until we hit the next command
	while (fgets(buffer, sizeof(buffer), in_file) && buffer[0] != '$') {
		// Check if we hit a directory
		if (buffer[0] == 'd') {
			// Find the next free index
			for (int i=0;i<max;i=i+1) {
				if (fs[i].name[0] == 0) {
					curr = i;
					break;
				}
			}
			// If we did grab the name
			if (sscanf(buffer, "dir %s\n", fs[curr].name) == 1) {
				// Assign the current index as the parent of the new dir
				fs[curr].parent = index;
			}
		}
		else
		{
			// Set curr to our current directory
			curr = index;
			// Otherwise the listing is a file with a filesize
			if (sscanf(buffer, "%d %s\n", &filesize, filename) == 2) {
				// Add the size
				fs[curr].size = fs[curr].size + filesize;
				// Go back through and update the parent sizes
				while (curr != 0) {
					curr = fs[curr].parent;
					fs[curr].size = fs[curr].size + filesize;
				}
			}
		}
	}
	// Go back to right before the next cmd
	// if we're not at the end of the file already
	if (!feof(in_file)) {
		fseek(in_file, -1*strlen(buffer), SEEK_CUR);
	}
	// End of function
	return;
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
	// and root fs
	struct directory* fs;
	char buffer[100];
	char name[20];
	int needed_space;
	int num_dir = 1;
	int curr = 0;
	int size_dir = 0;
	int delete_dir = 70000000;

	// We need to iterate over the file once to see how
	// big our array needs to be
	while(fgets(buffer, sizeof(buffer), in_file)) {
		// Check if the line defines a directory
		if (buffer[0] == 'd') {
			// If it does then we need to add to the number of children for 
			// this directory
			num_dir = num_dir + 1;
		}
	}
	// Go back to beginning of file
	rewind(in_file);
	// Allocate our array
	fs = (struct directory*)malloc(sizeof(struct directory)*num_dir);
	// Create our root
	strcpy(fs[0].name, "/");
	fs[0].parent = 0;
	fs[0].size = 0;
	// Initialize array
	for (int i=1;i<num_dir;i=i+1) {
		fs[i].name[0] = 0;
		fs[i].parent = 0;
		fs[i].size = 0;
	}

	// Go through the file line by line
	while(fgets(buffer, sizeof(buffer), in_file)) {
		// Check if the command is ls
		if (buffer[0] == '$' && buffer[2] == 'l') {
			ls(fs, curr, num_dir, in_file);
		}
		// Otherwise check if the file is cd
		else if (buffer[0] == '$' && buffer[2] == 'c') {
			// Grab the directory to cd into and cd into it
			if (sscanf(buffer, "$ cd %s\n", name) == 1) {
				curr = cd(name, fs, curr, num_dir);
			}
		}
	}
	// Calculate our needed space
	needed_space = 30000000 - (70000000 - fs[0].size);

	// Do our size checks
	for (int i=0;i<num_dir;i=i+1) {
		if (fs[i].size <= 100000) {
			size_dir = size_dir + fs[i].size;
		}
		if (fs[i].size > needed_space && fs[i].size < delete_dir) {
			delete_dir = fs[i].size;
		}
	}

	// Solve part one
	printf("Combined size of directories with a total size of at most 100000: %d\n", size_dir);
	// Print some debug info
	printf("Total needed space: %d\n", needed_space);
	// Solve part two
	printf("Size of directory to delete: %d\n", delete_dir);

	// Free all the memory we allocated
	free(fs);

	fclose(in_file);
	return EXIT_SUCCESS;
}
