#define NDEBUG
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "minhash.h"
#include "hashfactory.h"

int main() {
	srand(time(NULL));
	struct timeval start, end;
	double timeuse;
	gettimeofday(&start, NULL);
	
	load_computeSig();

	gettimeofday(&end, NULL);
	timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	timeuse /= 1000000;
	printf("MinHash Used Time:\t%lf s\n", timeuse);

	genCandbySig();
	computeLSH();

	gettimeofday(&end, NULL);
	timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	timeuse /= 1000000;
	printf("All Used Time:\t%lf s\n", timeuse);
}
