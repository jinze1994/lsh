#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "hashfactory.h"

#define USERCOUNT 10000LL
#define MAX_CONTACT_COUNT 1500
#define THRESHOLD 0.83
#define TOTALPAIR (USERCOUNT * (USERCOUNT - 1) / 2)

const char datapath[] = "./data/userdata.nt";
const char naivepath[] = "./data/naive.res";

struct HashTable ht[USERCOUNT];

int validpair = 0;

void naive() {
	FILE * fout = fopen(naivepath, "w");
	for(int a=0 ; a<USERCOUNT ; a++) {
		for(int b=a+1 ; b<USERCOUNT ; b++) {
			HashTable_reset(&ht[b]);
			int cnta = ht[a].top, cntb = ht[b].top, same = 0;

			char *p;
			int plen;
			while((p = HashTable_iterator(&ht[b], &plen)) != NULL)
				if(HashTable_exist(&ht[a], p, plen))
					same++;

			double sim = ((double)same) / (cnta + cntb - same);
			if(sim > THRESHOLD) {
				validpair++;
				fprintf(fout, "%d\t%d\n", a, b);
			}
		}

		if(a && a % (USERCOUNT / 10) == 0)
			printf("complete:\t%.2lf%%\n", (double)a * (USERCOUNT + USERCOUNT - a) / 2 / TOTALPAIR);
	}
}

int main() {
	printf("User Count:\t%lld\n", USERCOUNT);
	printf("Total Pair:\t%lld\n", TOTALPAIR);
	printf("Threshold:\t%lf\n", THRESHOLD);
	printf("Pre-Process\n");

	for(int i=0 ; i<USERCOUNT ; i++)
		HashTable_init(&ht[i], MAX_CONTACT_COUNT, 11);
	FILE * fin = fopen(datapath, "r");
	char buf[20];

	for(int i=0 ; i<USERCOUNT ; i++) {
		int num;
		if(fscanf(fin, "%d%d", &i, &num) != 2) {
			fprintf(stdout, "Read Format Error!\n");
			exit(1);
		}
		while(num--) {
			if(fscanf(fin, "%s", buf) != 1) {
				fprintf(stdout, "Read Format Error!\n");
				exit(1);
			}
			assert(strlen(buf) == 11);
			if(HashTable_insert(&ht[i], buf, 11) == 0) {
				fprintf(stdout, "Data Error:\tUser contact dup!\n");
				exit(1);
			}
		}
	}
	fclose(fin);
	printf("Build hashtable finished!\n");

	printf("Start timer\n");
	struct timeval start, end;
	double timeuse;
	gettimeofday(&start, NULL);
	
	naive();

	gettimeofday(&end, NULL);
	timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	timeuse /= 1000000;
	printf("Used Time:\t%lf s\n", timeuse);

	printf("Valid Pair:\t%d\n", validpair);

}
