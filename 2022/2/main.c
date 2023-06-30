#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char scores[10][3] = {
	"00\0",
	"BX\0",
	"CY\0",
	"AZ\0",
	"AX\0",
	"BY\0",
	"CZ\0",
	"CX\0",
	"AY\0",
	"BZ\0"
};

int getscore(char them, char us) {
	int score = 0;
	char game[3] = {them, us, '\0'};
	for (int i=1; i<11 && score == 0; i=i+1) {
		if (strcmp(game, scores[i]) == 0) {
			score = i;
		}
	}

	return score;
}

int main(int argc, char *argv[])
{
	FILE *in_file = fopen("input.txt", "r");
	if (in_file == NULL) {
		printf("Could not open input file\n");
		return EXIT_FAILURE;
	}

	int score = 0;
	char them;
	char us;
	while(fscanf(in_file, "%c %c\n", &them, &us) == 2) {
		score = score + getscore(them, us);
	}

	printf("The total score would be %d\n", score);

	return EXIT_SUCCESS;
}
