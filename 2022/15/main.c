#include <stdlib.h>
#include <stdio.h>

// Struct to store coordinate points
struct Point {
	int x;
	int y;
};

// Struct to store sensor/beacon pairs
struct Pair {
	struct Point sensor;
	struct Point beacon;
};

// Struct to store a list of coordinates with size
struct List {
	struct Point* coords;
	int curr;
	int size;
};

// Return absolute value of an int
int abs(int a) {
	return a >= 0 ? a : 0-a;
}

// Return the manhattan distance between two points
int mdist(struct Point a, struct Point b) {
	return abs(a.x-b.x)+abs(a.y-b.y);
}

// Return the point directly above
struct Point up(struct Point a) {
	a.y++;
	return a;
}
// Return the point directly below
struct Point down(struct Point a) {
	a.y--;
	return a;
}
// Return the point directly to the left
struct Point left(struct Point a) {
	a.x--;
	return a;
}
// Return the point directly to the right
struct Point right(struct Point a) {
	a.x++;
	return a;
}

// Return the point diagonally up and to the left
struct Point up_left(struct Point a) {
	a.x--;
	a.y++;
	return a;
}
// Return the point diagonally down and to the left
struct Point down_left(struct Point a) {
	a.x--;
	a.y--;
	return a;
}
// Return the point diagonally up and to the right
struct Point up_right(struct Point a) {
	a.x++;
	a.y++;
	return a;
}
// Return the point diagonally down and to the right
struct Point down_right(struct Point a) {
	a.x++;
	a.y--;
	return a;
}

// Checks if two points are equal
int pointeq(struct Point a, struct Point b) {
	return a.x == b.x && a.y == b.y ? 1 : 0;
}

// Return the point in the list that is not seen by all sensors in the pair set
struct Point signal(struct List list, struct Pair* set, int num_pairs, int limit) {
	// Declare stack vars
	struct Point signal;
	int num_valid = 0;
	// Iterate over the list
	for (int i=0; i<list.curr; i++) {
		// Make sure that the coordinate pair is within our limit of (0,limit)
		if (list.coords[i].x > limit || list.coords[i].y > limit || list.coords[i].x < 0 || list.coords[i].y < 0) {
			continue;
		}
		
		// Reset our number of sensors that can't see the point
		num_valid = 0;
		// Iterate over every sensor/beacon pair
		for (int j=0;j<num_pairs; j++) {
			// check if the point cannot be seen
			if (mdist(set[j].sensor, list.coords[i]) > mdist(set[j].sensor, set[j].beacon)) {
				num_valid++;
			}
		}
		// If every sensor cannot see this point it is the signal
		if (num_valid == num_pairs) {
			// Set our signal and stop looking
			signal = list.coords[i];
			break;
		}

	}
	// Return the signal
	return signal;
}

// This function finds all points with manhattan distance of 1 plus the manhattan
// distance between the sensor and beacon
struct List findmplusone(struct List list, struct Point b, struct Point s) {
	// Declare stack vars
	struct Point start;
	struct Point curr;

	// Calculate our target distance
	int dist = mdist(b, s)+1;
	// We haven't started
	int started = 0;
	// This figures out which direction has the right distance
	if (mdist(s, left(b)) == dist) {
		start = left(b);
	}
	else if (mdist(s, down(b)) == dist) {
		start = down(b);
	}
	else if (mdist(s, up(b)) == dist) {
		start = up(b);
	}
	else {
		start = right(b);
	}
	// Set curr equal to the starting spot
	curr = start;

	// We need to loop until we've gone around the entire circumference
	// and returned to start
	while (!started || !pointeq(start, curr)) {
		// We need to add curr to our list
		// so check if there's space
		if (list.curr < list.size) {
			// add curr to the list of coords
			list.coords[list.curr] = curr;
			list.curr++;
		}
		// Otherwise add another 100 points to the array
		// and then add curr
		else {
			list.size = list.size + 100;
			list.coords = (struct Point*)realloc(list.coords, sizeof(struct Point)*list.size);
			list.coords[list.curr] = curr;
			list.curr++;
		}
		// See if up and to the left is the next spot and that we haven't visited it
		if ((!started || !pointeq(up_left(curr), list.coords[list.curr-2])) && mdist(s, up_left(curr)) == dist) {
			curr = up_left(curr);
		}
		// See if down and to the left is the next spot and that we haven't visited it
		else if ((!started || !pointeq(down_left(curr), list.coords[list.curr-2])) && mdist(s, down_left(curr)) == dist) {
			curr = down_left(curr);
		}
		// See if up and to the right is the next spot and that we haven't visited it
		else if ((!started || !pointeq(up_right(curr), list.coords[list.curr-2])) && mdist(s, up_right(curr)) == dist) {
			curr = up_right(curr);
		}
		// Otherwise just go down and to the right as that's our only option
		else {
			curr = down_right(curr);
		}
		// Set that we have started going around the circumference
		started = 1;
	}

	// Return the updated list
	return list;
}

// This function solves part 1
int part1(struct Pair* input, int num_pairs, int max_x, int y) {
	// Declare stack vars
	int total = 0;
	struct Point test = {.y = y};

	// Iterate over a range that should contain all possible manhattan distances
	for (int i=-max_x; i<max_x*2; i++) {
		// Set our x value because the y is constant
		test.x = i;
		// Check if a sensor can see into this spot for every sensor
		for (int j=0; j<num_pairs; j++) {
			// Make sure its not a beacon already
			if (!pointeq(test, input[j].beacon)) {
				// Check to see if its less than equal to the distance because that means
				// the sensor can see it
				if (mdist(test, input[j].sensor) <= mdist(input[j].beacon, input[j].sensor)) {
					// Increase our total and stop checking for this loop since
					// we already validated that a sensor can see it
					total++;
					break;
				}
			}
		}
	}
	return total;
}

// This function solves part 2
long part2(struct Pair* input, int num_pairs, int max) {
	// Declare stack vars
	struct List valid;
	struct Point dbeacon;

	// Create our coordinate list, just start with 100 points per pair
	valid.coords = (struct Point*)malloc(sizeof(struct Point)*num_pairs*100);
	// Set our size of the list
	valid.size = num_pairs*100;
	// Set our current item to 0
	valid.curr = 0;
	// For every pair, find every point with manhattan distance of one plus the
	// manhattan distance between the beacon and sensor
	for (int i=0; i<num_pairs; i++) {
		valid = findmplusone(valid, input[i].beacon, input[i].sensor);
	}
	// Figure out where the distress beacon is
	// this should be the signal point that is not 
	// visiable by any sensor
	dbeacon = signal(valid, input, num_pairs, max);
	// Free our dynamic memory
	free(valid.coords);
	// Return the calculated frequency
	return (long)dbeacon.x*4000000+(long)dbeacon.y;
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
	char buffer[100];
	int num_pairs = 0;
	int curr = 0;
	int high_x = 0;
	long freq = 0;
	struct Pair* input;

	// Get number of sensor/beacon pairs in file
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		if (buffer[0] != '\n') {
			num_pairs++;
		}
	}
	// Go back to beginning of the file
	rewind(in_file);
	// Allocate our memory
	input = (struct Pair*)malloc(sizeof(struct Pair)*num_pairs);

	// Iterate over the file a line at a time
	while(fgets(buffer, sizeof(buffer), in_file) != NULL) {
		// Check if the line is blank
		if (buffer[0] != '\n') {
			// See if we can get the ints out of the line
			if (sscanf(buffer, "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d", 
						&input[curr].sensor.x, &input[curr].sensor.y,
						&input[curr].beacon.x, &input[curr].beacon.y) == 4) {
				// Check if this is the highest x
				if (input[curr].sensor.x > high_x || !curr) {
					high_x = input[curr].sensor.x;
				}
				// Check again for beacon
				if (input[curr].beacon.x > high_x) {
					high_x = input[curr].beacon.x;
				}
				// Continue to the next pair
				curr++;
			}
		}
	}

	// Solve part 1
	printf("The number of spaces that cannot contain a beacon is %d\n", part1(input, num_pairs, high_x, 2000000));
	// Solve part 2
	printf("The frequency is %ld\n", part2(input, num_pairs, 4000000));

	// Free our dynamic memory
	free(input);

	// Close our file
	fclose(in_file);

	return EXIT_SUCCESS;
}
