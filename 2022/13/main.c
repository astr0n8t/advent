#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// NestedList structure
// can either be an int or a list
struct NestedList {
	char type;
	int value;
	struct NestedList* list;
};

// Static constants
#define STARTLIST '['
#define ENDLIST ']'
#define SEPARATOR ','

#define LIST 'L'
#define INT 'I'

#define CORRECT 0
#define INCORRECT 1
#define UNDEFINED -1


// Gets size of current list assuming that the start of the list bracket is at
// list[index]
size_t size(char list[300], int index) {
	// Stack vars
	int result = 0;
	int depth = 1;

	// Check if the list is empty
	if (list[index] == STARTLIST && list[index+1] == ENDLIST) {
		result = 0;
	}
	else {
		// Otherwise there will always be at least one item
		result = 1;
		// Iterate over the string until we're at the end of our list
		for (int i = index+1; i<strlen(list) && depth != 0; i=i+1) {
			// Check if a new list has started
			if (list[i] == STARTLIST) {
				depth = depth + 1;
			}
			// Check if we hit the end of a list
			else if (list[i] == ENDLIST) {
				depth = depth - 1;
			}
			// If we hit a comma and we're at the correct depth
			// this means theres another item in our list
			else if (list[i] == SEPARATOR && depth == 1) {
				result = result + 1;
			}
		}
	}

	return result;
}

// Parses a string and returns a pointer to an equivalent NestedList structure
struct NestedList* parse(char inputlist[300], int index) {
	// Get the overall size of the list
	size_t listsize = size(inputlist, index);
	// Initialize our pointer to store it
	struct NestedList* list = NULL;
	// Check if there are any items in the list
	if (listsize) {
		// Initialize some memory to store the items
		list = (struct NestedList*)malloc(sizeof(struct NestedList)*listsize);
		// Go to the next character in the string
		index = index + 1;
		// Set our starting depth to 0
		int depth = 0;

		// For every item in this list
		for (int i=0; i<listsize; i=i+1) {
			// Check if the current character is the start of a new list
			if (inputlist[index] == STARTLIST) {
				// Create the sublist by calling size and this function again
				// with the index set properly
				list[i].type = LIST;
				list[i].value = size(inputlist, index);
				list[i].list = parse(inputlist, index);
				// Our current index is wrong so we need to update it
				depth = 1;
				// Search for the next item in our current depth list
				while(depth) {
					index = index + 1;
					if (inputlist[index] == STARTLIST) {
						depth = depth + 1;
					}
					else if (inputlist[index] == ENDLIST) {
						depth = depth - 1;
					}
				}
				// We're currently on ']' so we need to skip past '],' to the next item
				index = index + 2;
			}
			// Otherwise the value is not a list
			else {
				// Check if the value is not 10
				if (inputlist[index+1] == SEPARATOR || inputlist[index+1] == ENDLIST) {
					list[i].type = INT;
					list[i].value = inputlist[index] - '0';
					list[i].list = NULL;
					index = index + 2;
				}
				// Otherwise the value is 10
				else {
					list[i].type = INT;
					list[i].value = 10;
					list[i].list = NULL;
					index = index + 3;
				}
			}
		}
	}

	return list;
}

// Prints out a list in the given input format
// useful for debugging
void printlist(struct NestedList* inputlist) {
	// Check if we're printing a list
	if (inputlist->type == LIST) {
		// Print an open bracket
		printf("%c", STARTLIST);
		// Iterate over every item in the list
		for (int i=0; i<inputlist->value; i=i+1) {
			// If its another list call this function recursively
			if (inputlist->list[i].type == LIST) {
				printlist(&inputlist->list[i]);
			}
			// Otherwise print out the decimal value
			else {
				printf("%d", inputlist->list[i].value);
			}
			// Check if we need to print a comma
			if (i != inputlist->value-1) {
				printf("%c", SEPARATOR);
			}
		}
		// Print a closing bracket
		printf("%c", ENDLIST);
	}
	// If its not a list just print the value
	else {
		printf("%d", inputlist->value);
	}
	return;
}

// Actually checks if two nested lists are in the right order
int compare(struct NestedList* left, struct NestedList* right) {
	// Always start with undefined
	int correct = UNDEFINED;

	// Check if the two elements are both ints
	if (left->type == INT && right->type == INT) {
		// Check if they're in the right order
		if (left->value < right->value) {
			correct = CORRECT;
		}
		// Check if they're in the wrong order
		else if (left->value > right->value) {
			correct = INCORRECT;
		}
		// Otherwise its still undefined
	}
	// Check if the two elements are both lists
	else if (left->type == LIST && right->type == LIST) {
		// Check if they're equal size
		if (left->value == right->value) {
			// If they are just recursively call this function on each pair of items
			for (int i=0; i<right->value && correct == UNDEFINED; i=i+1) {
				correct = compare(&left->list[i], &right->list[i]);
			}
		}
		// Check if there are more values on the left side
		else if (left->value > right->value) {
			// Call this function for every item for as many items are in the right list
			for (int i=0; i<right->value && correct == UNDEFINED; i=i+1) {
				correct = compare(&left->list[i], &right->list[i]);
			}
			// If the value is undefined then its actually incorrect since the
			// right side ran out of items first
			if (correct == UNDEFINED) {
				correct = INCORRECT;
			}
		}
		else {
			// Call the function for every item for as many items are in the left list
			for (int i=0; i<left->value && correct == UNDEFINED; i=i+1) {
				correct = compare(&left->list[i], &right->list[i]);
			}
			// If the value is undefined then its actually correct since the 
			// left side ran out of items first
			if (correct == UNDEFINED) {
				correct = CORRECT;
			}
		}
	}
	// Check if the left is a list but the right is an int
	else if (left->type == LIST && right->type == INT) {
		// Convert the right into a list of one item
		right->list = (struct NestedList*)malloc(sizeof(struct NestedList));
		right->list[0].value = right->value;
		right->list[0].type = INT;
		right->type = LIST;
		right->value = 1;
		// Re-run the comparison function
		correct = compare(left, right);
	}
	// Check if the left is an int but the right is a list
	else if (left->type == INT && right->type == LIST) {
		// Convert the left to a list of one item
		left->list = (struct NestedList*)malloc(sizeof(struct NestedList));
		left->list[0].value = left->value;
		left->list[0].type = INT;
		left->type = LIST;
		left->value = 1;
		// Re-run the comparison function
		correct = compare(left, right);
	}

	// Return either correct, incorrect, or undefined
	return correct;
}

// This function frees all memory allocated by a NestedList
void freelist(struct NestedList* inputlist) {
	// Iterate over every item in the current list
	for (int i=0; i<inputlist->value; i=i+1) {
		// If the subitem is a list then call the freelist function on it
		if (inputlist->list[i].type == LIST) {
			freelist(&inputlist->list[i]);
		}
	}
	// If we allocated our list then free it
	if (inputlist->value) {
		free(inputlist->list);
	}
	return;
}

// This function fully copies a NestedList
void deepcopy(struct NestedList* dst, struct NestedList* src) {
	// Check if the src is a list
	if (src->type == LIST) {
		// If it is make the dst a list as well
		dst->value = src->value;
		dst->type = LIST;
		// If there are any items in the list
		if (dst->value) {
			// Allocate our memory for the list
			dst->list = (struct NestedList*)malloc(sizeof(struct NestedList)*(dst->value));
			// Call the function recursively for each item in the list
			for (int i=0; i<dst->value; i=i+1) {
				deepcopy(&dst->list[i], &src->list[i]);
			}
		}
	}
	// Otherwise its just an int so copy the value and set the type
	else {
		dst->type = INT;
		dst->value = src->value;
	}
	
	return;
}

// "Quick" bubblesort that sorts our list
void sort(struct NestedList* inputlist, int size) {
	// We need three NestedLists
	struct NestedList tmp;
	struct NestedList left;
	struct NestedList right;
	// Do bubblesort loops
	for (int i=0;i<size-1; i=i+1) {
		for (int j=0; j<size-i-1; j=j+1) {
			// Deepcopy our variables to check into left and right
			deepcopy(&left, &inputlist[j]);
			deepcopy(&right, &inputlist[j+1]);
			// Compare left and right
			// this modifies the NestedList which is why we need the deepcopy
			if (compare(&left, &right) == INCORRECT) {
				// Perform the swap (doesn't need to be a deepcopy)
				tmp = inputlist[j];
				inputlist[j] = inputlist[j+1];
				inputlist[j+1] = tmp;
			}
			// Since we deepcopied we need to free our memory
			freelist(&left);
			freelist(&right);
		}
	}

	return;
}

// Checks if list is either [[2]] or [[6]]
int checkdivider(struct NestedList* inputlist) {
	int correct = INCORRECT;
	if (inputlist->type == LIST) {
		if (inputlist->value == 1) {
			if (inputlist->list[0].type == LIST) {
				if (inputlist->list[0].value == 1) {
					if (inputlist->list[0].list[0].type == INT) {
						if (inputlist->list[0].list[0].value == 2 || inputlist->list[0].list[0].value == 6) {
							correct = CORRECT;
						}
					}
				}
			}
		}
	}
	return correct;
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
	int sum = 0;
	int divider_index = 1;
	int index = 0;
	char list1[300];
	char list2[300];
	struct NestedList left = {.type = LIST, .value = 0};
	struct NestedList right = {.type = LIST, .value = 0};
	struct NestedList* sorted;
	int size_sorted = 2;
	int sorted_index = 2;

	// Get size of our array
	while(fgets(list1, sizeof(list1), in_file) != NULL) {
		if (list1[0] != '\n') {
			size_sorted = size_sorted + 1;
		}
	}
	rewind(in_file);

	// Allocate our memory
	sorted = (struct NestedList*)malloc(sizeof(struct NestedList)*size_sorted);

	// Add divider blocks
	strncpy(list1, "[[2]]\0", 6); 
	strncpy(list2, "[[6]]\0", 6); 
	sorted[0].type = LIST;
	sorted[0].value = 1;
	sorted[0].list = parse(list1, 0);
	sorted[1].type = LIST;
	sorted[1].value = 1;
	sorted[1].list = parse(list2, 0);

	// Iterate over the file a line at a time
	while(fgets(list1, sizeof(list1), in_file) != NULL) {
		// we're done with getting input
		if (list1[0] != '\n') {
			if (fgets(list2, sizeof(list2), in_file) == NULL) {
				continue;
			}
			// Increment our index every two lines
			index = index + 1;

			// Initialize left and right using the strings
			left.value = size(list1, 0);
			left.list = parse(list1, 0);
			right.value = size(list2, 0);
			right.list = parse(list2, 0);

			// Deepcopy these values into our array before we call compare
			deepcopy(&sorted[sorted_index], &left);
			deepcopy(&sorted[sorted_index+1], &right);
			sorted_index = sorted_index + 2;

			// Check if they're in the right order
			if (compare(&left, &right) == CORRECT) {
				sum = sum + index;
			}

			// Free left and right
			freelist(&left);
			freelist(&right);
			// Reset our sizes
			left.value = 0;
			right.value = 0;
		}

	}

	// Solve part one
	printf("The sum of indices is %d\n", sum);

	// Sort the array of lists
	sort(sorted, size_sorted);
	// Get the product of the divider indices
	for (int i=0; i<size_sorted; i=i+1) {
		if (checkdivider(&sorted[i]) == CORRECT) {
			divider_index = divider_index*(i+1);
		}
	}

	// Solve part two
	printf("The product of divider indices is %d\n", divider_index);

	// Free our memory
	for (int i=0; i<size_sorted; i=i+1) {
		freelist(&sorted[i]);
	}
	free(sorted);

	// Close our file
	fclose(in_file);
	return EXIT_SUCCESS;
}
