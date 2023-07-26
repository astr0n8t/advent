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
	int current_valve = 0;
	int target_valve = 0;

	for (int min=0; min<30; min++) {
		for (int i=0; i<valves.size; i++) {
			if (valves.list[i].state == VALVEOPEN) {
				released_pressure = released_pressure + valves.list[i].rate;
			}
		}

		// Check if we're at target
		if (current_valve == target_valve) {
			// Check if we need to open target
			if (valves.list[current_valve].state == VALVECLOSE && valves.list[current_valve].flow != 0) {
			}
			// Find new target
			else {
			}
		}
		// Otherwise move towards target
		else {
		}



	}

	return released_pressure;
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
