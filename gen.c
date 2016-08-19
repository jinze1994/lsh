#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hashfactory.h"

#define USERCOUNT 10000
#define TELECOUNT (USERCOUNT*2)

#define MIN_CONTACT_COUNT 200
#define MAX_CONTACT_COUNT 1500

#define RAND(a, b) ((int)(rand() % (b-a+1) + a))

const char datapath[] = "./data/userdata.nt";

char telebook[TELECOUNT][20];

int main() {
	printf("gen user:\t%d\n", USERCOUNT);
	printf("random contact range:\t%d .. %d\n", MIN_CONTACT_COUNT, MAX_CONTACT_COUNT);

	srand(time(NULL));
// gen telebook
	for(int i=0 ; i<TELECOUNT ; i++) {
		for(int j=0 ; j<11 ; j++)
			telebook[i][j] = RAND('0', '9');
//			telebook[i][j] = '0';
		telebook[i][11] = '\0';
	}

	printf("Start Generate Userdata ...\n");

// gen userdata
	int genentry = 0, flater = 0;
	FILE * outp = fopen(datapath, "w");

	struct HashTable hash;
	HashTable_init(&hash, MAX_CONTACT_COUNT, 11);

	for(int i=0 ; i<USERCOUNT ; i++) {
		HashTable_clear(&hash);

		fprintf(outp, "%d", i);
		int contactcnt = RAND(MIN_CONTACT_COUNT, MAX_CONTACT_COUNT);
		int sep = RAND(0, MAX_CONTACT_COUNT);

		int eachflater = 0;
		for(int j=0 ; j<contactcnt ; j++) {
			const char * p = telebook[(i + sep + j) % TELECOUNT];
			if(HashTable_insert(&hash, p, 11) == 0)
				eachflater++;
		}

		contactcnt -= eachflater;
		flater += eachflater;
		genentry += contactcnt;
		fprintf(outp, "\t%d", contactcnt);
		char *p;
		int plen;
		while((p = HashTable_iterator(&hash, &plen)) != NULL) {
			fwrite("\t", 1, 1, outp);
			fwrite(p, plen, 1, outp);
		}
		fprintf(outp, "\n");
		fflush(outp);

		if(i && i % (USERCOUNT / 10) == 0)
			printf("complete:\t%.2lf%%\n", (double)i / USERCOUNT);
	}
	printf("Closing File ...\n");
	fclose(outp);

	printf("flater entry:\t%d\n", flater);
	printf("gen entry:\t%d\n", genentry);

}
