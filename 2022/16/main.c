#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Define values for open and closed valves
#define VALVEOPEN 0
#define VALVECLOSE 1

// Definea valve
struct Valve {
	char name[3];
	int rate;
	int leads[5];
	int num_leads;
	int state;
};

// Define a list of valves
struct ValveList {
	struct Valve* list;
	int size;
};

// Function to return a deepcopy of a ValveList
struct ValveList deepcopy(struct ValveList src) {
	// Copy the stack elements
	struct ValveList dst = src;
	// Allocate new memoory for our ValveList
	dst.list = (struct Valve*)malloc(sizeof(struct Valve)*dst.size);
	// Copy each value in the list
	for (int i=0; i<dst.size; i++) {
		dst.list[i] = src.list[i];
	}
	// Return the new ValveList
	return dst;
}

// Function to pre-compute the costs of each traversal
void calculatecost(struct ValveList valves, int** costtable) {

	// Define a stack var for later
	int new_val = 1;

	// Set costs of depth 1
	for (int i=0; i<valves.size; i++) {
		for (int j=0; j<valves.list[i].num_leads; j++) {
			costtable[i][valves.list[i].leads[j]] = 1;
		}
	}

	// Calculate until we're confident we've found the shortest path
	while (new_val) {
		new_val = 0;
		// Iterate over the costtable
		for (int i=0; i<valves.size; i++) {
			for (int j=0; j<valves.size; j++) {
				// Check if the cost exists
				if (costtable[i][j]) {
					// If it does iterate over every lead in j
					for (int k=0; k<valves.list[j].num_leads; k++) {
						// For every lead check if the distance exists and that i is not equal to the lead
						if(!costtable[i][valves.list[j].leads[k]] && i != valves.list[j].leads[k]) {
							// Otherwise the shortest distance to this lead is the cost of i to j plus one
							costtable[i][valves.list[j].leads[k]] = costtable[i][j] + 1;
							new_val = 1;
						}
						// Check if our way is faster
						else if (costtable[i][j]+1 < costtable[i][valves.list[j].leads[k]]) {
							// If it is set it as the new shortest distance
							costtable[i][valves.list[j].leads[k]] = costtable[i][j] + 1;
							new_val = 1;
						}
					}
				}
			}
		}
	}

	return;
}

// Recursive solution for part1
int part1(struct ValveList prev, int** costtable, int time, int curr) {
	// If time is out then we can't get any more pressure released
	if (time <= 0) {
		return 0;
	}
	// Create a deepcopy of the valvelist to work with
	struct ValveList valves = deepcopy(prev);
	// Set the current valve to open
	valves.list[curr].state = VALVEOPEN;
	// Create variables to hold the potential costs
	int best_cost = 0;
	int curr_cost = 0;
	// Iterate over all of our valves
	for (int i=0; i<valves.size; i++) {
		// Check if it has a good pressure release rate and that the valve
		// can be opened
		if (valves.list[i].rate > 0 && valves.list[i].state == VALVECLOSE) {
			// Try this valve as the next one, reducing time by the cost to
			// get to that valve minus one to open the valve
			curr_cost = part1(valves, costtable, time-1-costtable[curr][i], i);
			// Check if that releases more pressure than our previous best
			if (curr_cost > best_cost) {
				best_cost = curr_cost;
			}
		}
	}
	// Free our deepcopy memory
	free(valves.list);

	// Return the best path plus the current amount of pressure that 
	// would be released at the current valve
	return best_cost+prev.list[curr].rate*time;
}

// Recursive solution for part2
int part2(struct ValveList prev, int** costtable, int time, int time2, int curr, int curr2) {
	// If time is out then we can't get any more pressure released
	if (time2 <= 0) {
		return 0;
	}
	// Create a deepcopy of our valvelist
	struct ValveList valves = deepcopy(prev);
	// Set the current valve to open
	valves.list[curr2].state = VALVEOPEN;
	// Create variables to hold the potential costs
	int best_cost = 0;
	int curr_cost = 0;
	// Iterate over all of our valves
	for (int i=0; i<valves.size; i++) {
		// Check if it has a good pressure release rate and that the valve
		// can be opened
		if (valves.list[i].rate > 0 && valves.list[i].state == VALVECLOSE) {
			// Call this function again, but set the secondary variables to primary and set
			// our next valve as the new secondary variables
			// This allows the elephant to run concurrently to us
			curr_cost = part2(valves, costtable, time2, time-1-costtable[curr][i], curr2, i);
			// Check if that releases more pressure than our previous best
			if (curr_cost > best_cost) {
				best_cost = curr_cost;
			}
		}
	}
	// Free our deepcopy memory
	free(valves.list);

	// Return the best path plus the current amount of pressure that 
	// would be released at the current valve
	return best_cost+prev.list[curr2].rate*time2;
}

// Finds the index of the specified named valve
int findvalveindex(char name[3], struct ValveList valves) {
	// Iterate until we find the valve
	int index = 0;
	for (int i=0; i<valves.size; i++) {
		if (strcmp(name, valves.list[i].name) == 0) {
			index = i;
			break;
		}
	}

	// Return the valve index
	return index;
}

// Debug function that prints out the cost table
void printcosttable(struct ValveList valves, int** costtable) {
	// Print out valve names
	for (int i=0; i<valves.size; i++) {
			printf("%s ", valves.list[i].name);
	}
	printf("\n");
	// Print out the costs
	for (int i=0; i<valves.size; i++) {
		for (int j=0; j<valves.size; j++) {
			printf("%2d ", costtable[i][j]);
		}
		printf("\n");
	}
	return;
}

// Function to parse the input file and return
// a valvelist
struct ValveList processinput(FILE *in_file) {
	// Create our stack vars
	char buffer[100];
	char* tmp;
	char tmp2[3];
	int curr = 0;
	struct ValveList valves = {.size = 0};

	// Get number of sensor/beacon pairs in file
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		if (buffer[0] != '\n') {
			valves.size++;
		}
	}
	// Go back to beginning of the file
	rewind(in_file);
	valves.list = (struct Valve*)malloc(sizeof(struct Valve)*valves.size);

	// Get all of our valves and flow rates
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		// Check if the line is blank
		if (buffer[0] != '\n') {
			// Get the name of the valve
			valves.list[curr].name[0] = buffer[6];
			valves.list[curr].name[1] = buffer[7];
			valves.list[curr].name[2] = 0;
			// Set the valve to closed
			valves.list[curr].state = VALVECLOSE;

			// Get the rate of the valve
			tmp = strtok(buffer, ";");
			tmp = strtok(tmp, "=");
			tmp = strtok(NULL, "=");
			sscanf(tmp, "%d", &valves.list[curr].rate);
			// Move to the next valve in the valve list
			curr++;
		}
	}
	rewind(in_file);
	// Set our tunnel leads
	curr = 0;
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		// Check if the line is blank
		if (buffer[0] != '\n') {
			tmp = strtok(buffer, ";");
			tmp = strtok(NULL, ";");
			// Get all of the leads for that valve
			valves.list[curr].num_leads = 0;
			for (int i=strlen(tmp)-1; i>0; i--) {
				if (tmp[i] == '\n' || tmp[i] == ',') {
					// Set a tmp name to locate the valve
					tmp2[0] = tmp[i-2];
					tmp2[1] = tmp[i-1];
					tmp2[2] = 0;
					// Find the valve indexand store it as a lead
					valves.list[curr].leads[valves.list[curr].num_leads] = findvalveindex(tmp2, valves);
					// Increase oour number of leaves
					valves.list[curr].num_leads++;
				}
			}
			// Keep going through our valve list
			curr++;
		}
	}
	// return the valve list
	return valves;
}

int main(int argc, char *argv[])
{
	// Read in our input file
	FILE *in_file = fopen("input.txt", "r");
	if (in_file == NULL) {
		printf("Could not open input file!");
		exit(-1);
	}
	// Declare stack vars
	// Get our valvelist from the input file
	struct ValveList valves = processinput(in_file);
	// Find our starting valve index
	int start_index = findvalveindex("AA", valves);
	// Allocate memory for our costtable and zero out the 2d array
	int** costtable = (int**)malloc(sizeof(int*)*valves.size+sizeof(int)*valves.size*valves.size);
	for (int i=0;i<valves.size;i++) {
		costtable[i] = (int*)((int*)(costtable+valves.size)+valves.size*i);
		for (int j=0;j<valves.size;j++) {
			costtable[i][j] = 0;
		}
	}

	// Calculate the cost to get to each vertex
	calculatecost(valves, costtable);

	// Solve part 1
	printf("The most pressure by ourself that can be released is %d\n", part1(valves, costtable, 30, start_index));
	// Solve part 2
	printf("The most pressure with the elephant that can be released is %d\n", part2(valves, costtable, 26, 26, start_index, start_index));

	// Clean up memory
	free(valves.list);
	free(costtable);
	// Close our file
	fclose(in_file);

	return EXIT_SUCCESS;
}
