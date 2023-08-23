#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A snafu is just a 24 character string
typedef struct snafu {
	char val[25];
} Snafu;

// A list of snafus
typedef struct snafulist {
	Snafu* array;
	size_t size;
	size_t max;
} SnafuList;

// Computes a^b
// doesn't work for negative powers
long lpow(long a, long b) {
	// Stack vars
	long orig = a;
	// Return 0 if negative since we don't support them
	if (b < 0) {
		return 0;
	}
	// If power is 0 return 1
	if (!b) {
		return 1;
	}
	// Otherwise compute the value of a^b
	for (long i=1; i<b; i++) {
		a *= orig;
	}
	return a;
}

// Converts a snafu to a long
long snafutol(Snafu in) {
	// Stack vars
	long result = 0;
	int curr = 0;
	long marker = 0;

	// Go backwards over the snafu
	for (int i=strlen(in.val)-1; i>-1; i--) {
		// Check what digit were on and set the marker
		// appropriately
		switch (in.val[i]) {
			case '=':
				marker = -2;
				break;
			case '-':
				marker = -1;
				break;
			case '0':
				marker = 0;
				break;
			case '1':
				marker = 1;
				break;
			case '2':
				marker = 2;
				break;
		}
		// The value at this spot is the value of that digit 
		// multiplied by its current position in base 5
		result += marker * lpow(5, curr++);
	}

	return result;
}

// Converts a long to a snafu
Snafu longtos(long in) {
	// Stack vars
	Snafu result;
	Snafu output;
	int curr = 0;
	long digit = 0;
	long carry = 0;

	// Iterate until in is 0
	while(in) {
		// Calculate our digit which is the remainder of
		// in divided by 5 and then add the carry bit
		digit = (in % 5)+carry;
		// Divide in by 5
		in /= 5;
		// Figure out what are value is and if we need to 
		// set the carry bit
		switch (digit) {
			case 0:
				result.val[curr] = '0';
				carry = 0;
				break;
			case 1:
				result.val[curr] = '1';
				carry = 0;
				break;
			case 2:
				result.val[curr] = '2';
				carry = 0;
				break;
			case 3:
				result.val[curr] = '=';
				carry = 1;
				break;
			case 4:
				result.val[curr] = '-';
				carry = 1;
				break;
			// Case 5 is in case the remainder was 4 but the carry
			// bit is also set, we need to set our digit to 0 and 
			// set the carry bit again
			case 5:
				result.val[curr] = '0';
				carry = 1;
				break;
		}
		// Increment our current digit
		curr++;
	}
	// Add another 1 to the end of result if the carry bit 
	// was set
	if (carry) {
		result.val[curr++] = '1';
	}
	// Null terminate result
	result.val[curr] = 0;
	// Our answer is actually backwrds so we need to reverse the result
	// into the output variable
	for (int i=0; i<strlen(result.val); i++) {
		output.val[strlen(result.val)-i-1] = result.val[i];
	}
	// Null terminate output
	output.val[curr] = 0;

	return output;
}

// Solve the sum
Snafu solve(SnafuList snafus) {
	// Stack var
	long sum = 0;
	// For each snafu convert it to a long 
	// and add it to the sum
	for (int i=0; i<snafus.size; i++) {
		sum += snafutol(snafus.array[i]);
	}
	// Return the sum as a snafu
	return longtos(sum);
}

// Processes the input file and stores the result in the given
// snafulist
void processinput(char* filename, SnafuList* output) {
	// Read in our file
	FILE *in_file = fopen(filename, "r");
	if (in_file == NULL) {
		printf("Could not open input file\n");
		exit(EXIT_FAILURE);
	}

	// Stack vars
	char buffer[100];
	// Zero-initialize our snafulist
	output->max = 0;
	output->size = 0;

	// Figure out size of snafulist
	while(fgets(buffer, sizeof buffer, in_file) != NULL) {
		// For every non-empty line add another snafu
		if (buffer[0] != '\n') {
			output->max++;
		}
	}
	// Go back to the beginning of the file
	rewind(in_file);

	// Allocate our memory
	output->array = malloc((sizeof *output->array)*output->max);

	// Go back over the file line by line
	while(fgets(buffer, sizeof buffer, in_file) != NULL) {
		// Make sure the line is not blank
		if (buffer[0] != '\n') {
			// Copy the buffer into the value of the snafu
			strncpy(output->array[output->size].val, buffer, strlen(buffer)-1);
			// Null terminate the snafu value
			output->array[output->size++].val[strlen(buffer)] = 0;
		}
	}

	// Close our file in case we want to call this function again
	fclose(in_file);
	return;
}

int main(int argc, char *argv[])
{
	// Stack vars
	SnafuList snafus;
	Snafu answer;
	// Get our snafu list
	processinput("input.txt", &snafus);
	// Get the sum of all the snafus
	answer = solve(snafus);
	// Solve by printing out the snafu sum
	printf("The sum in snafu is %s\n", answer.val);

	// Free our memory
	free(snafus.array);
	return EXIT_SUCCESS;
}
