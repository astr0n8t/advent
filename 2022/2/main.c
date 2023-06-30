#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// We can calculate the scores as an index of
// an array.  Each score ranges from 1-9 and
// is unqiue.
static char scores1[10][3] = {
	"00\0",
	"BX\0", //1
	"CY\0", //2
	"AZ\0", //3
	"AX\0", //4
	"BY\0", //5
	"CZ\0", //6
	"CX\0", //7
	"AY\0", //8
	"BZ\0"  //9
};

// We can do the same thing for the second
// array as well
static char scores2[10][3] = {
	"00\0",
	"BX\0", //1
	"CX\0", //2
	"AX\0", //3
	"AY\0", //4
	"BY\0", //5
	"CY\0", //6
	"CZ\0", //7
	"AZ\0", //8
	"BZ\0"  //9
};

// Function iterates over the given score array
// checks if its the right score
// If it is the index of the array is returned
// since it is the score for that part
int getscore(char them, char us, char scores[10][3]) {
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
	// Read in our file
	FILE *in_file = fopen("input.txt", "r");
	if (in_file == NULL) {
		printf("Could not open input file\n");
		return EXIT_FAILURE;
	}

	// Declare stack vars
	int score1 = 0;
	int score2 = 0;
	char them;
	char us;
	// Iterate over the file, grabbing two characters
	// at a time
	while(fscanf(in_file, "%c %c\n", &them, &us) == 2) {
		// Call the getscore for each part
		score1 = score1 + getscore(them, us, scores1);
		score2 = score2 + getscore(them, us, scores2);
	}

	// Output the two scores
	printf("The total score for part 1 would be %d\n", score1);
	printf("The total score for part 2 would be %d\n", score2);

	fclose(in_file);
	return EXIT_SUCCESS;
}
