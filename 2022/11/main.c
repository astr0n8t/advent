#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct monkey {
	// Maximise size of worry we can store
	unsigned long long* items;
	int num_items;
	int inspected;
	int div;
	char op;
	char r_op;
	int n_op;
	int true;
	int false;
} monkey;


// This function actually acts out the logic of a round
void runround(struct monkey* pack, int num, int divisor, int mod) {
	// tmp stack vars
	int r_op = 0;
	int index = 0;

	// Iterate over every monkey
	for (int i=0; i<num; i=i+1) {
		// Iterate over every item in the monkey's inventory
		for (int j=0; j<pack[i].num_items; j=j+1) {
			// Increment number of items inspected
			pack[i].inspected = pack[i].inspected + 1;
			// Choose our right operand
			if (pack[i].r_op == 'o') {
				r_op = pack[i].items[j];
			}
			else {
				r_op = pack[i].n_op;
			}

			// Compute the new worry value
			// we use a modulus to reduce the worry value
			if (pack[i].op == '+') {
				pack[i].items[j] = ((pack[i].items[j] + r_op) / divisor) % mod;
			}
			else {
				pack[i].items[j] = ((pack[i].items[j] * r_op) / divisor) % mod;
			}

			// Find which monkey to send the item to
			if (pack[i].items[j] % pack[i].div == 0) {
				index = pack[i].true;
			}
			else {
				index = pack[i].false;
			}

			// Make sure we can fit the item in the monkey's inventory
			if (pack[index].num_items + 1 > num*5) {
				pack[index].items = (unsigned long long*)realloc(pack[index].items, sizeof(unsigned long long)*(pack[index].num_items+1));
			}
			// Send the item to the new monkey
			pack[index].items[pack[index].num_items] = pack[i].items[j];
			pack[index].num_items = pack[index].num_items + 1;

			// Shift our array back
			for (int k=j; k<pack[i].num_items; k=k+1) {
				pack[i].items[k] = pack[i].items[k+1];
			}
			j = j - 1;
			pack[i].num_items = pack[i].num_items - 1;
		}
	}

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
	char buffer[100];
	char buff2[5];
	int num_monkey = 0;
	struct monkey* pack;
	struct monkey* pack2;
	int curr = 0;
	int mod = 1;
	int inspectors1[2] = {0, 0};
	unsigned long long inspectors2[2] = {0, 0};

	// Go through file and get number of monkeys
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		if (buffer[0] == 'M') {
			num_monkey = num_monkey + 1;
		}
	}
	rewind(in_file);

	// Allocate memory
	pack = (struct monkey*)malloc(sizeof(struct monkey)*num_monkey);
	pack2 = (struct monkey*)malloc(sizeof(struct monkey)*num_monkey);
	// pre-allocate monkey inventory
	for (int i=0; i<num_monkey; i=i+1) {
		pack[i].items = (unsigned long long*)malloc(sizeof(unsigned long long)*num_monkey*5);
	}

	// Parse the monkey file
	// this bit is kind of gross but it works assuming
	// the format never changes
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		if (buffer[0] == 'M') {
			sscanf(buffer, "Monkey %d:\n", &curr);
		}
		else if (buffer[2] == 'S') {
			pack[curr].num_items = (strlen(buffer) - 17)/4;
			if (pack[curr].num_items > 5*num_monkey) {
				pack[curr].items = (unsigned long long*)realloc(pack[curr].items, sizeof(unsigned long long)*pack[curr].num_items);
			}
			for (int i=0; i<pack[curr].num_items; i=i+1) {
				for (int j=0; j<4; j=j+1) {
					buff2[j] = buffer[17+i*4+j];
				}
				sscanf(buff2, "%llu", &pack[curr].items[i]);
			}
		}
		else if (buffer[2] == 'O') {
			pack[curr].op = buffer[23];
			if (buffer[25] == 'o') {
				pack[curr].r_op = 'o';
			}
			else {
				pack[curr].r_op = 'i';
				for (int i=24; i<strlen(buffer); i=i+1) {
					buff2[i-24] = buffer[i];
				}
				sscanf(buff2, "%d", &pack[curr].n_op);
			}
		}
		else if (buffer[2] == 'T') {
			for (int i=20; i<strlen(buffer); i=i+1) {
				buff2[i-20] = buffer[i];
			}
			sscanf(buff2, "%d", &pack[curr].div);
		}
		else if (buffer[8] == 'r') {
			for (int i=28; i<strlen(buffer); i=i+1) {
				buff2[i-28] = buffer[i];
			}
			sscanf(buff2, "%d", &pack[curr].true);
		}
		else if (buffer[8] == 'a') {
			for (int i=29; i<strlen(buffer); i=i+1) {
				buff2[i-29] = buffer[i];
			}
			sscanf(buff2, "%d", &pack[curr].false);
		}
	}
	// Copy into the second pack and re-allocate arrays
	memcpy(pack2, pack, sizeof(struct monkey)*num_monkey);
	for (int i=0; i<num_monkey; i=i+1) {
		// Allocate and copy our values, but we need to make sure we 
		// didnt resize our arrays
		if (pack[i].num_items < 5*num_monkey) {
			pack2[i].items = (unsigned long long*)malloc(sizeof(unsigned long long)*num_monkey*5);
			memcpy(pack2[i].items, pack[i].items, sizeof(unsigned long long)*num_monkey*5);
		}
		else {
			pack2[i].items = (unsigned long long*)malloc(sizeof(unsigned long long)*pack[i].num_monkey*5);
			memcpy(pack2[i].items, pack[i].items, sizeof(unsigned long long)*pack[i].num_monkey*5);
		}
	}
	// Calculate our modulus to reduce storage size of worry values
	for (int i=0; i<num_monkey; i=i+1) {
		mod = pack[i].div * mod;
	}

	// Run 20 rounds for part one
	for (int i=0; i<20; i=i+1) {
		runround(pack, num_monkey, 3, mod);
	}
	// Run 10000 rounds for part two
	for (int i=0; i<10000; i=i+1) {
		runround(pack2, num_monkey, 1, mod);
	}

	// Grab the two highest inspected pairs
	// could be reduced to a function but alas I'm lazy
	for (int i=0; i<num_monkey; i=i+1) {
		if (pack[i].inspected > inspectors1[0]) {
			if(inspectors1[1] == 0) {
				inspectors1[1] = pack[i].inspected;
			}
			else {
				if (inspectors1[0] > inspectors1[1]) {
					inspectors1[1] = inspectors1[0];
				}
				inspectors1[0] = pack[i].inspected;
			}
		}
		if (pack2[i].inspected > inspectors2[0]) {
			if(inspectors2[1] == 0) {
				inspectors2[1] = pack2[i].inspected;
			}
			else {
				if (inspectors2[0] > inspectors2[1]) {
					inspectors2[1] = inspectors2[0];
				}
				inspectors2[0] = pack2[i].inspected;
			}
		}
	}

	// Output our solutions
	printf("Level of monkey business part 1: %d\n", inspectors1[0]*inspectors1[1]);
	printf("Level of monkey business part 2: %llu\n", inspectors2[0]*inspectors2[1]);


	// Free our memory
	for (int i=0; i<num_monkey; i=i+1) {
		free(pack[i].items);	
		free(pack2[i].items);	
	}
	free(pack);
	free(pack2);

	fclose(in_file);
	return EXIT_SUCCESS;
}
