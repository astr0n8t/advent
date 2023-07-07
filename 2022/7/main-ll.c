#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Create our directory datatype
// its a doubly linked list kinda
// similar to an inode cause if it
// aint broke
struct directory {
	char name[20];
	int size;
	int num_children;
	struct directory* children;
	struct directory* parent;
};

// This function is necessary to free all the children of a given
// directory.  We use it by passing the root fs into it and 
// it should free all of the child directory memory allocations
void free_children(struct directory* curr) {
	// If we're not root and don't have any children we don't 
	// need to  call free here
	if (strcmp(curr->name, "/") == 0 || curr->num_children > 0) {
		// Iterate over the children and recursively call 
		// to get all children freed
		for(int i=0; i<curr->num_children; i=i+1) {
			// If the pointer is not void then it has
			// some children which could also potentially have
			// children
			if (&curr->children[i] != NULL) {
				free_children(&curr->children[i]);
			}
		}
		// Free the children
		free(curr->children);
	}
	// Always return null
	return;
}

// This solves the first part of the problem
// returning the total number of directories witha size
// less than or equal to the limit
int directory_size_search(struct directory* curr, int limit) {
	// Start with size 0 always
	int size = 0;
	// Go through all children for the current directory
	for(int i=0; i<curr->num_children; i=i+1) {
		// If the pointer is not void then it has
		// some children which could also potentially have
		// children
		if (&curr->children[i] != NULL) {
			// Add the size of the child to our size
			size = size + directory_size_search(&curr->children[i], limit);
		}
	}
	// Check if our size is less than the limit
	if (curr->size <= limit) {
		// If it is add it to our total
		size = size + curr->size;
	}

	// Return the total
	return size;
}

// This solves the second part of the problem
// returning the size of the smallest directory
// we can delete fo give us the neeeded amount of space
int directory_delete_search(struct directory* curr, int needed) {
	// Set our size to the max filesystem space
	// this solves issues if the current dir size 
	// is undefined 
	int size = 70000000;
	// Have a variable to store the child size into
	int child_size = 0;
	// Check if our size is enough to satisfy the needed space
	if (curr->size >= needed) {
		// If it is set the size to our current dir size
		size = curr->size;
	}

	// Go through all the children 
	for(int i=0; i<curr->num_children; i=i+1) {
		// If the pointer is not void then it has
		// some children which could also potentially have
		// children
		if (&curr->children[i] != NULL) {
			// Get the size of the child
			child_size = directory_delete_search(&curr->children[i], needed);
			// Check if its less than our size
			if (child_size < size) {
				// If it is, it becomes the new best size
				size = child_size;
			}
		}
	}

	// Return the final size
	return size;
}

// This function recursively updates the size of the parent directory
// its a nice little hack to not need to worry about dealing with
// the size later and also is the requirement for the struct to be 
// a doubly linked list
void set_parent_size(struct directory* curr, int size) {
	// If we're at the root do nothing
	if (curr->parent != NULL) {
		// cd to the parent dir
		curr = curr->parent;
		// Add the size to the parent dir
		curr->size = curr->size + size;
		// Keep going up the fs
		return set_parent_size(curr, size);
	}
	// End of function
	return;
}

// This function handles the cd command
struct directory* cd(struct directory* curr, char name[20]) {
	// check if we need to cd to the root
	if (strcmp(name, "/") == 0) {
		// If we do just go back until we hit it
		while (curr->parent != NULL) {
			curr = curr->parent;
		}
	}
	// Check if we need to go back a directory
	else if	(strcmp(name, "..") == 0) {
		// If we do make sure we're not already at root
		// and then go up one
		if (curr->parent != NULL) {
			curr = curr->parent;
		}
	} 
	else {
		// In all other casees we need to find the currect directory
		for (int i=0; i<curr->num_children; i=i+1) {
			// Check if the child name is the correct name
			if (strcmp(curr->children[i].name, name) == 0) {
				// Go to the child directory
				curr = &curr->children[i];
				// Exit the loop early
				i = curr->parent->num_children;
			}
		}
	}
	// The fallback case will always be the current directory
	return curr;
}

// This function handles the ls command
void ls(struct directory* curr, FILE* in_file) {
	// We need quite a few stackvars here
	char buffer[100];
	char filename[20];
	int filesize = 0;
	int size_listing = 0;
	int num_children = 0;

	// Iterate through the file till we hit the next command
	while(fgets(buffer, sizeof(buffer), in_file) && buffer[0] != '$') {
		// Each time keep track of the size of the line for later
		size_listing = size_listing + strlen(buffer);
		// Check if the line defines a directory
		if (buffer[0] == 'd') {
			// If it does then we need to add to the number of children for 
			// this directory
			num_children = num_children + 1;
		}
	}

	// Go back to beginning of directory listing
	if (!feof(in_file)) {
		// We have to check if the file is empty cause otherwise we
		// can get stuck in an infinite loop of the same command
		fseek(in_file, (-1*size_listing)-strlen(buffer), SEEK_CUR);
	} 
	else {
		// If we're at the end of the file don't worry about the last buffer
		fseek(in_file, -1*size_listing, SEEK_CUR);
	}

	// Allocate our children
	if (num_children > 0) {
		curr->children = (struct directory*)malloc(sizeof(struct directory)*num_children);
	}
	else {
		// Fallback case is just a null pointer for the child
		curr->children = NULL;
	}

	// Run through the ls listing again
	while(fgets(buffer, sizeof(buffer), in_file) && buffer[0] != '$') {
		// Check if we hit a directory
		if (buffer[0] == 'd') {
			// If we did grab the name
			if (sscanf(buffer, "dir %s\n", curr->children[curr->num_children].name) == 1) {
				// Assign the name to the next child of the current directory
				curr->children[curr->num_children].parent = curr;
				// Increment the number of children in the directory
				curr->num_children = curr->num_children + 1;
			}
		}
		else
		{
			// Otherwise the listing is a file with a filesize
			if (sscanf(buffer, "%d %s\n", &filesize, filename) == 2) {
				// Add the filesize to the directory size
				curr->size = curr->size + filesize;
				// Go back through and update the parent sizes
				set_parent_size(curr, filesize);
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
	struct directory* fs = malloc(sizeof(struct directory));
	strcpy(fs->name, "/");
	fs->parent = NULL;
	char buffer[100];
	struct directory* current_directory = fs;
	char name[20];
	int unused_space;

	// Go through the file line by line
	while(fgets(buffer, sizeof(buffer), in_file)) {
		// Check if the command is ls
		if (buffer[0] == '$' && buffer[2] == 'l') {
			ls(current_directory, in_file);
		}
		// Otherwise check if the file is cd
		else if (buffer[0] == '$' && buffer[2] == 'c') {
			// Grab the directory to cd into and cd into it
			if (sscanf(buffer, "$ cd %s\n", name) == 1) {
				current_directory = cd(current_directory, name);
			}
		}
	}
	// Calculate our unused space
	unused_space = 70000000 - fs->size;
	// Solve part one
	printf("Combined size of directories with a total size of at most 100000: %d\n", directory_size_search(fs, 100000));
	// Print some debug info
	printf("Total needed space: %d\n", 30000000-unused_space);
	// Solve part two
	printf("Size of directory to delete: %d\n", directory_delete_search(fs, 30000000-unused_space));

	// Free all the memory we allocated
	free_children(fs);
	free(fs);

	fclose(in_file);
	return EXIT_SUCCESS;
}
