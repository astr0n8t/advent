#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Determines the value of monkeys with dependencies
#define OPERATION -1

// The monkey struct
typedef struct monkey {
	char name[5];
	char operation;
	char lname[5];
	char rname[5];
	double val;
} Monkey;

// The monkey list
typedef struct monkeylist {
	Monkey* array;
	size_t size;
	size_t max;
} MonkeyList;

// Returns the index of a monkey or if it cant find it
// just 0
int indexof(MonkeyList list, char name[5]) {
	for (int i=0; i<list.size; i++) {
		if (strcmp(list.array[i].name, name) == 0) {
			return i;
		}
	}
	return 0;
}

// The recursive solution function
double solve(MonkeyList list, Monkey current) {
	// Stack vars
	double value = 0;
	// If the current monkey has no dependencies
	if (current.val != OPERATION) {
		// Just set the value equal to the monkeys value
		value = current.val;
	}
	// Otherwise we have to proess the dependencies
	else {

		// Get the left and right values by calling this function again
		double lval = solve(list, list.array[indexof(list, current.lname)]);
		double rval = solve(list, list.array[indexof(list, current.rname)]);

		// Perform the monkey operation on the left and right values
		switch(current.operation) {
			case '+':
				value = lval + rval;
				break;
			case '-':
				value = lval - rval;
				break;
			case '*':
				value = lval * rval;
				break;
			case '/':
				value = lval / rval;
				break;
		}
	}
	// Return the determined monkey value
	return value;
}

// Solve part1
long part1(MonkeyList monkeys) {
	// Solve for root
	return (long)solve(monkeys, monkeys.array[indexof(monkeys, "root")]);
}

// Solve part2
long part2(MonkeyList monkeys) {
	// Get the index of the human monkey
	int hindex = indexof(monkeys, "humn");
	// Set humn to 0
	monkeys.array[hindex].val = 0;

	// Get the root monkey
	Monkey root = monkeys.array[indexof(monkeys, "root")];
	// Get the index of the left monkey from root
	int left = indexof(monkeys, root.lname);
	// Get the index of the right monkey from root
	int right = indexof(monkeys, root.rname);
	// Get the total of the left
	double l = solve(monkeys, monkeys.array[left]);
	// Get the total of the right
	double r = solve(monkeys, monkeys.array[right]);
	// Get the difference froom the two sdes
	double diff = l - r;
	// Set our stepped human value 
	monkeys.array[hindex].val = 10000;
	// Solve for left and right again
	l = solve(monkeys, monkeys.array[left]);
	r = solve(monkeys, monkeys.array[right]);
	// Now lets compute a starting candidate value for the human,
	// which is just the total difference between left and right divided by
	// the average difference of left and right for our stepped values
	monkeys.array[hindex].val = (long)((l-r)/((diff - (l-r))/monkeys.array[hindex].val));
	// This should have put us within about five decimal places of the right answer
	// so we can fuzz the answer now

	// Declare a divisor
	float divisor = 1;
	// Iterate until the two sides are equal
	// we should converge on a value eventually
	while ((l = solve(monkeys, monkeys.array[left])) !=  (r = solve(monkeys, monkeys.array[right]))) {
		// Check if the difference is positive
		if (l-r > 0) {
			// Add to the human value, reducing the value as we go
			monkeys.array[hindex].val += 1000000/divisor++;
		}
		else {
			// Subtract to the human value, reducing the value as we go
			monkeys.array[hindex].val -= 1000000/divisor++;
		}
	}

	// Return the value as a long
	return (long)monkeys.array[hindex].val;
}

// Processes the selected input file and stores the result in the 
// given MonkeyList
void processinput(char* filename, MonkeyList* monkeys) {
	// Read in our file
	FILE *in_file = fopen(filename, "r");
	if (in_file == NULL) {
		printf("Could not open input file\n");
		exit(EXIT_FAILURE);
	}

	// Stack var
	char buffer[100];

	// Figure out how many monkeys there are
	while(fgets(buffer, sizeof buffer, in_file) != NULL) {
		monkeys->max++;
	}
	// Go back to the beginning of the file
	rewind(in_file);

	// Aloocate oour monkeys
	monkeys->array = malloc((sizeof *monkeys->array)*monkeys->max);

	// Go over the file a line at a time
	while(fgets(buffer, sizeof buffer, in_file) != NULL) {
		// Check if the monkey has dependencies
		if (sscanf(buffer, "%4s: %4s %c %4s\n", monkeys->array[monkeys->size].name, 
					monkeys->array[monkeys->size].lname, &monkeys->array[monkeys->size].operation,
					monkeys->array[monkeys->size].rname) == 4) {
			// If it does, set its value to be operationo
			monkeys->array[monkeys->size++].val = OPERATION;
		}
		// Otherwise check if the monkey has a value
		else if (sscanf(buffer, "%4s: %lf\n", monkeys->array[monkeys->size].name,
					&monkeys->array[monkeys->size].val) == 2) {
			// If it is just increase our size
			monkeys->size++;
		}
		// Otherwise something went wrong
		else {
			printf("Error: could not process line: %s\n", buffer);
		}
	}
	// Close our file in case we want to call this function again
	fclose(in_file);
	return;
}

int main(int argc, char *argv[])
{
	// Stavk ar
	MonkeyList monkeys = {.array=NULL,.size=0,.max=0};
	// Get our monkey input
	processinput("input.txt", &monkeys);

	// Solve part1
	printf("The root monkey yelled %ld\n", part1(monkeys));
	// Solve part2
	printf("The value for humn should be %ld\n", part2(monkeys));

	// Free our allocated array
	free(monkeys.array);
	return EXIT_SUCCESS;
}
