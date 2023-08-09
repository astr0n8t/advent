#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct robot {
	int ore;
	int clay;
	int obsidian;
} Robot;

typedef struct blueprint {
	Robot ore;
	Robot clay;
	Robot obsidian;
	Robot geode;
} Blueprint;

typedef struct inventory {
	int ore;
	int clay;
	int obsidian;
	int geode;
} Inventory;

typedef struct blueprintlist {
	Blueprint* array;
	size_t size;
} BlueprintList;

typedef struct history {
	Inventory collected;
	Inventory items;
	Inventory robots;
} History;

typedef struct historylist {
	History* array;
	size_t size;
	size_t max;
} HistoryList;

int cmphistory(const void* av, const void* bv) {
	History* a = (History*)av;
	History* b = (History*)bv;
	if (a->collected.ore-b->collected.ore + 
	   (a->collected.clay-b->collected.clay)*16 + 
	   (a->collected.obsidian-b->collected.obsidian)*512 + (a->collected.geode-b->collected.geode)*1024 > 0) {
			return 0;
	}
	return 1;
}

void sortinto(HistoryList* a, HistoryList* b) {
	qsort(b->array, b->size, sizeof(History), cmphistory);

	a->size = 0;
	for (int i=0; i<a->max && i<b->size; i++) {
		a->array[a->size++] = b->array[i];
	}

	return;
}

void histinsert(HistoryList* history, History current) {
	if (history->size == history->max) {
		history->max += 1000;
		history->array = realloc(history->array, sizeof(History)*(history->max));
	}
	history->array[history->size++] = current;
}

void printhist(History a) {
	printf("History entry val: %d ", a.items.geode);
	printf("items: %d %d %d %d ", a.items.ore, a.items.clay, a.items.obsidian, a.items.geode);
	printf("robots: %d %d %d %d ", a.robots.ore, a.robots.clay, a.robots.obsidian, a.robots.geode);
	printf("\n");
}

int solve(Blueprint template, int minutes, int num_states) {
	int max = 0;
	HistoryList states = {.size=0, .max=num_states};
	states.array = malloc(sizeof(History)*states.max);
	Inventory blank = {.ore=0,.clay=0,.obsidian=0,.geode=0};
	HistoryList new_states = {.size=0, .max=num_states};
	History current;
	History current_bot;

	states.array[0] = (History){.items=blank, .robots=blank, .collected=blank};
	states.array[0].robots.ore = 1;
	states.size = 1;
	new_states.array = malloc(sizeof(History)*new_states.max);
	for (int i=0; i<minutes; i++) {
		new_states.size = 0;
		for (int j=0; j<states.size; j++) {
			current = states.array[j];

			current.items.ore += current.robots.ore;
			current.items.clay += current.robots.clay;
			current.items.obsidian += current.robots.obsidian;
			current.items.geode += current.robots.geode;

			current.collected.ore += current.robots.ore;
			current.collected.clay += current.robots.clay;
			current.collected.obsidian += current.robots.obsidian;
			current.collected.geode += current.robots.geode;

			histinsert(&new_states, current);

			if (states.array[j].items.ore >= template.geode.ore && states.array[j].items.obsidian >= template.geode.obsidian) {
				current_bot = current;
				current_bot.robots.geode++;
				current_bot.items.ore -= template.geode.ore;
				current_bot.items.obsidian -= template.geode.obsidian;
				histinsert(&new_states, current_bot);
			}

			if (states.array[j].items.ore >= template.obsidian.ore && states.array[j].items.clay >= template.obsidian.clay) {
				current_bot = current;
				current_bot.robots.obsidian++;
				current_bot.items.ore -= template.obsidian.ore;
				current_bot.items.clay -= template.obsidian.clay;
				histinsert(&new_states, current_bot);
			}
			if (states.array[j].items.ore >= template.clay.ore) {
				current_bot = current;
				current_bot.robots.clay++;
				current_bot.items.ore -= template.clay.ore;
				histinsert(&new_states, current_bot);
			}

			if (states.array[j].items.ore >= template.ore.ore) {
				current_bot = current;
				current_bot.robots.ore++;
				current_bot.items.ore -= template.ore.ore;
				histinsert(&new_states, current_bot);
			}
		}


		sortinto(&states, &new_states);

		//for (int i=0; i<states.size; i++) {printhist(states.array[i]);}
		//printf("\n");

	}

	for (int i=0; i<states.size; i++) {
		if (states.array[i].items.geode > max) {
			max = states.array[i].items.geode;
		}
	}

	free(new_states.array);
	free(states.array);

	return max;
}


int part1(BlueprintList blueprints, int num_minutes) {
	int sum = 0;
	int num_geodes = 0;

	for (int i=0; i<blueprints.size; i++) {
		num_geodes = solve(blueprints.array[i], num_minutes, 1024);
		sum += (i+1)*num_geodes;
	}
	return sum;
}

int part2(BlueprintList blueprints, int num_minutes) {
	int mul = 1;
	int num_geodes = 0;

	for (int i=0; i<3 && i != blueprints.size; i++) {
		num_geodes = solve(blueprints.array[i], num_minutes, 16384);
		mul *= num_geodes;
	}
	return mul;
}

BlueprintList processinput(char* filename) {
	// Read in our input file
	FILE *in_file = fopen(filename, "r");
	if (in_file == NULL) {
		printf("Could not open input file!");
		exit(-1);
	}

	BlueprintList output = {.size=0};
	char buffer[500];
	char* split;
	
	// Get number of blueprints
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		if (buffer[0] != '\n') {
			output.size++;
		}
	}
	rewind(in_file);

	output.array = malloc(sizeof(Blueprint)*output.size);

	for (int i=0; i<output.size && fgets(buffer, sizeof(buffer), in_file) != NULL; i++) {
		if (buffer[0] == '\n') {
			i--;
			continue;
		}
		// Get everything after the :
		split = strtok(buffer, ":");
		split = strtok(NULL, ":");

		split = strtok(split, ".");
		if (sscanf(split, " Each ore robot costs %d ore", &output.array[i].ore.ore) != 1) {
			printf("Error processing input: %s\n", split);
		}

		split = strtok(NULL, ".");
		if (sscanf(split, " Each clay robot costs %d ore", &output.array[i].clay.ore) != 1) {
			printf("Error processing input: %s\n", split);
		}

		split = strtok(NULL, ".");
		if (sscanf(split, " Each obsidian robot costs %d ore and %d clay", 
					&output.array[i].obsidian.ore, &output.array[i].obsidian.clay) != 2) {
			printf("Error processing input: %s\n", split);
		}

		split = strtok(NULL, ".");
		if (sscanf(split, " Each geode robot costs %d ore and %d obsidian", 
					&output.array[i].geode.ore, &output.array[i].geode.obsidian) != 2) {
			printf("Error processing input: %s\n", split);
		}
	}

	fclose(in_file);

	return output;
}

int main(int argc, char *argv[])
{
	BlueprintList blueprints = processinput("input.txt");
	printf("Sum of quality levels of all blueprints: %d\n", part1(blueprints, 24));
	printf("Multiplication of geodes mined of first 3 blueprints: %d\n", part2(blueprints, 32));

	free(blueprints.array);
	return EXIT_SUCCESS;
}
