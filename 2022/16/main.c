#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define VALVEOPEN 0
#define VALVECLOSE 1

struct Valve {
	char name[3];
	int rate;
	int leads[5];
	int num_leads;
	int state;
};

struct ValveList {
	struct Valve* list;
	int size;
};

struct ValveList deepcopy(struct ValveList src) {
	struct ValveList dst = src;
	dst.list = (struct Valve*)malloc(sizeof(struct Valve)*dst.size);
	for (int i=0; i<dst.size; i++) {
		dst.list[i] = src.list[i];
	}
	return dst;
}

void calculatecost(struct ValveList valves, int** costtable) {

	// Set costs of depth 1
	for (int i=0; i<valves.size; i++) {
		for (int j=0; j<valves.list[i].num_leads; j++) {
			costtable[i][valves.list[i].leads[j]] = 1;
		}
	}

	// Calculate depths up to the number of valves
	for (int x=0; x<valves.size; x++) {
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
						}
					}
				}
			}
		}
	}

	return;
}

int opportunitycost(struct ValveList prev, int** costtable, int time, int curr, int dst) {
	if (time-1 <= costtable[curr][dst]) {
		return 0;
	}
	
	struct ValveList valves = deepcopy(prev);
	int base_cost = valves.list[dst].rate*(time-1-costtable[curr][dst]);
	valves.list[dst].state = VALVEOPEN;
	int best_cost = 0;
	int curr_cost = 0;
	for (int i=0; i<valves.size; i++) {
		if (valves.list[i].rate > 0 && valves.list[i].state == VALVECLOSE) {
			curr_cost = opportunitycost(valves, costtable, time-1-costtable[curr][dst], dst, i);
			if (curr_cost > best_cost) {
				best_cost = curr_cost;
			}
		}
	}
	free(valves.list);

	return best_cost+base_cost;
}


void printcosttable(struct ValveList valves, int** costtable) {
	for (int i=0; i<valves.size; i++) {
		for (int j=0; j<valves.size; j++) {
			printf("%d ", costtable[i][j]);
		}
		printf("\n");
	}
	return;
}


int findvalveindex(char name[3], struct ValveList valves) {
	int index = 0;
	for (int i=0; i<valves.size; i++) {
		if (strcmp(name, valves.list[i].name) == 0) {
			index = i;
			break;
		}
	}

	return index;
}

struct ValveList processinput(FILE *in_file) {
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
			valves.list[curr].name[0] = buffer[6];
			valves.list[curr].name[1] = buffer[7];
			valves.list[curr].name[2] = 0;
			valves.list[curr].state = VALVECLOSE;

			tmp = strtok(buffer, ";");
			tmp = strtok(tmp, "=");
			tmp = strtok(NULL, "=");
			sscanf(tmp, "%d", &valves.list[curr].rate);
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
			valves.list[curr].num_leads = 0;
			for (int i=strlen(tmp)-1; i>0; i--) {
				if (tmp[i] == '\n' || tmp[i] == ',') {
					tmp2[0] = tmp[i-2];
					tmp2[1] = tmp[i-1];
					tmp2[2] = 0;
					valves.list[curr].leads[valves.list[curr].num_leads] = findvalveindex(tmp2, valves);
					valves.list[curr].num_leads++;
				}
			}
			curr++;
		}
	}
	return valves;
}

int part1(struct ValveList valves) {
	int released_pressure = 0;
	int most_pressure = 0;
	int start_index = 0;

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

	
	for (int i=0; i<valves.size; i++) {
		if (strcmp("AA", valves.list[i].name) == 0) {
			start_index = i;
			break;
		}
	}

	for (int i=0; i<valves.size; i++) {
		released_pressure = opportunitycost(valves, costtable, 30, start_index, i);
		if (released_pressure > most_pressure) {
			most_pressure = released_pressure;
		}
	}

	free(costtable);
	return most_pressure;
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
	struct ValveList valves = processinput(in_file);

	printf("The most pressure that can be released is %d\n", part1(valves));

	// Clean up memory
	free(valves.list);
	// Close our file
	fclose(in_file);

	return EXIT_SUCCESS;
}
