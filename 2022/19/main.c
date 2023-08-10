#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Stuct to hold robot type
typedef struct robot {
	int ore;
	int clay;
	int obsidian;
} Robot;

// Struct to hold blueprint which
// contains different types of robots
typedef struct blueprint {
	Robot ore;
	Robot clay;
	Robot obsidian;
	Robot geode;
} Blueprint;

// Struct to hold general purpose inventory
typedef struct inventory {
	int ore;
	int clay;
	int obsidian;
	int geode;
} Inventory;

// Struct to hold a simple list of Blueprints
typedef struct blueprintlist {
	Blueprint* array;
	size_t size;
} BlueprintList;

// Struct to hold state of items collected,
// itemas in the inventory, and robots
typedef struct state {
	Inventory collected;
	Inventory items;
	Inventory robots;
} State;

// Struct to hold a list of states
typedef struct statelist {
	State* array;
	size_t size;
	size_t max;
} StateList;

// Comparison function to pass to qsort
int cmpstate(const void* av, const void* bv) {
	// Takes in pointers to states
	State* a = (State*)av;
	State* b = (State*)bv;
	// Check our collection waits to see which state is better
	if (a->collected.ore-b->collected.ore + 
	   (a->collected.clay-b->collected.clay)*16 + 
	   (a->collected.obsidian-b->collected.obsidian)*512 + (a->collected.geode-b->collected.geode)*1024 > 0) {
			// B is better than A
			return 0;
	}
	// A is better than B
	return 1;
}

// Sorts b into a
void sortinto(StateList* a, StateList* b) {
	// Sort B
	qsort(b->array, b->size, sizeof(State), cmpstate);

	// Reset A
	a->size = 0;
	// Copy as much of B that we can fit into A
	for (int i=0; i<a->max && i<b->size; i++) {
		a->array[a->size++] = b->array[i];
	}

	return;
}

// Inserts a state into a statelist
void stateinsert(StateList* state, State current) {
	// Check if our statelist is at the max size
	if (state->size == state->max) {
		// Add 1000 entries to the statelist
		state->max += 1000;
		// Re-allocate our memory with the new size
		state->array = realloc(state->array, sizeof(State)*(state->max));
	}
	// Insert the state into the statelist
	state->array[state->size++] = current;

	return;
}

// Solution function for a specific minute
int solve(Blueprint template, int minutes, int num_states) {
	// Stack vars
	int max = 0;
	State current;
	State current_bot;
	Inventory blank = {.ore=0,.clay=0,.obsidian=0,.geode=0};
	StateList states = {.size=0, .max=num_states};
	StateList new_states = {.size=0, .max=num_states};

	// Allocate our arrays
	states.array = malloc(sizeof(State)*states.max);
	new_states.array = malloc(sizeof(State)*new_states.max);

	// Set the first item in the primary statelist
	// The only item is a single ore robot
	states.array[0] = (State){.items=blank, .robots=blank, .collected=blank};
	states.array[0].robots.ore = 1;
	states.size = 1;

	// Iterate for every minute
	for (int i=0; i<minutes; i++) {
		// Reset our new_states list
		new_states.size = 0;
		// Iterate for every state in our states list
		for (int j=0; j<states.size; j++) {
			// Copy the array item into current
			current = states.array[j];

			// Increment our inventory
			current.items.ore += current.robots.ore;
			current.items.clay += current.robots.clay;
			current.items.obsidian += current.robots.obsidian;
			current.items.geode += current.robots.geode;

			// Increment our collected history
			current.collected.ore += current.robots.ore;
			current.collected.clay += current.robots.clay;
			current.collected.obsidian += current.robots.obsidian;
			current.collected.geode += current.robots.geode;

			// Insert the updated state into our new_states list
			stateinsert(&new_states, current);

			// Check if we can build a geode bot
			if (states.array[j].items.ore >= template.geode.ore && states.array[j].items.obsidian >= template.geode.obsidian) {
				// Copy current to current_bot
				current_bot = current;
				// Add our geode bot
				current_bot.robots.geode++;
				// Remove the cost of the geode bot from our inventory
				current_bot.items.ore -= template.geode.ore;
				current_bot.items.obsidian -= template.geode.obsidian;
				// Insert the state into the new_states list
				stateinsert(&new_states, current_bot);
			}

			// Check if we can build an obsidian bot
			if (states.array[j].items.ore >= template.obsidian.ore && states.array[j].items.clay >= template.obsidian.clay) {
				// Copy current to current_bot
				current_bot = current;
				// Add our obsidian bot
				current_bot.robots.obsidian++;
				// Remove the cost of the obsidian bot from our inventory
				current_bot.items.ore -= template.obsidian.ore;
				current_bot.items.clay -= template.obsidian.clay;
				// Insert the state into the new states list
				stateinsert(&new_states, current_bot);
			}

			// Check if we can build a clay bot
			if (states.array[j].items.ore >= template.clay.ore) {
				// Copy current to current_bot
				current_bot = current;
				// Add our clay bot
				current_bot.robots.clay++;
				// Remove the cost of the clay bot from our inventory
				current_bot.items.ore -= template.clay.ore;
				// Insert the state into the new_states list
				stateinsert(&new_states, current_bot);
			}

			// Check if we can build a ore bot
			if (states.array[j].items.ore >= template.ore.ore) {
				// Copy current to current_bot
				current_bot = current;
				// Add our ore bot
				current_bot.robots.ore++;
				// Remove the cost of the ore bot from our inventory
				current_bot.items.ore -= template.ore.ore;
				// Insert the state into the new_states list
				stateinsert(&new_states, current_bot);
			}
		}
		// Sort new_states into states
		// This effectively selects only the best states up to the number
		// specified by num_states
		sortinto(&states, &new_states);
	}

	// Find our max number of geodes in the state array
	for (int i=0; i<states.size; i++) {
		if (states.array[i].items.geode > max) {
			max = states.array[i].items.geode;
		}
	}

	// Free our allocated memory
	free(new_states.array);
	free(states.array);

	// Return the max number of geodes
	return max;
}


// Solve part1
int part1(BlueprintList blueprints, int num_minutes) {
	// Stack vars
	int sum = 0;
	int num_geodes = 0;

	// For every blueprint
	for (int i=0; i<blueprints.size; i++) {
		// Try to solve with a max states of 1024
		num_geodes = solve(blueprints.array[i], num_minutes, 1024);
		// Add the max number of geodes to our sum
		sum += (i+1)*num_geodes;
	}
	// Return the sum of geode maxes
	return sum;
}

// Solve part2
int part2(BlueprintList blueprints, int num_minutes) {
	// Stack vars
	int mul = 1;
	int num_geodes = 0;

	// For the first three blueprints or until we hit the end of the blueprint list
	for (int i=0; i<3 && i != blueprints.size; i++) {
		// Try to solve with a max states of 16384
		num_geodes = solve(blueprints.array[i], num_minutes, 16384);
		// Multiply the max number of geodes to our stored mul
		mul *= num_geodes;
	}
	// Return the product of geode maxes
	return mul;
}

// Process the specified input file
BlueprintList processinput(char* filename) {
	// Read in our input file
	FILE *in_file = fopen(filename, "r");
	if (in_file == NULL) {
		printf("Could not open input file!");
		exit(-1);
	}

	// Stack vars
	BlueprintList output = {.size=0};
	char buffer[500];
	char* split;
	
	// Get number of blueprints
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		if (buffer[0] != '\n') {
			output.size++;
		}
	}
	// Go back to begining of the file
	rewind(in_file);

	// Allocate our memory
	output.array = malloc(sizeof(Blueprint)*output.size);

	// Iterate over the entire file
	for (int i=0; i<output.size && fgets(buffer, sizeof(buffer), in_file) != NULL; i++) {
		// If the line is blank just ignore it
		if (buffer[0] == '\n') {
			i--;
			continue;
		}
		// Get everything after the :
		split = strtok(buffer, ":");
		split = strtok(NULL, ":");

		// Split on each period
		split = strtok(split, ".");
		// Get the cost of ore bots
		if (sscanf(split, " Each ore robot costs %d ore", &output.array[i].ore.ore) != 1) {
			printf("Error processing input: %s\n", split);
		}

		// Get the next period delimited spot
		split = strtok(NULL, ".");
		// Get the cost of clay bots
		if (sscanf(split, " Each clay robot costs %d ore", &output.array[i].clay.ore) != 1) {
			printf("Error processing input: %s\n", split);
		}

		// Get the next period delimited spot
		split = strtok(NULL, ".");
		// Get the cost of obsidian bots
		if (sscanf(split, " Each obsidian robot costs %d ore and %d clay", 
					&output.array[i].obsidian.ore, &output.array[i].obsidian.clay) != 2) {
			printf("Error processing input: %s\n", split);
		}

		// Get the next period delimited spot
		split = strtok(NULL, ".");
		// Get the cost of clay bots
		if (sscanf(split, " Each geode robot costs %d ore and %d obsidian", 
					&output.array[i].geode.ore, &output.array[i].geode.obsidian) != 2) {
			printf("Error processing input: %s\n", split);
		}
	}

	// Close our file in case we want to call processinput again
	fclose(in_file);

	// Return the blueprintlist
	return output;
}

int main(int argc, char *argv[])
{
	// Create our blueprint list from the input file
	BlueprintList blueprints = processinput("input.txt");
	// Solve part1
	printf("Sum of quality levels of all blueprints: %d\n", part1(blueprints, 24));
	// Solve part2
	printf("Multiplication of geodes mined of first 3 blueprints: %d\n", part2(blueprints, 32));

	// Free our blueprintlist memory
	free(blueprints.array);
	return EXIT_SUCCESS;
}
