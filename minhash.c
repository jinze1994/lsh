#include "minhash.h"
#include "hashfactory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

const char * pathdata = "./data/userdata.nt";

uint32_t sig[USERCOUNT][HASHCOUNT];

struct HashFamily hashfamily;

struct HashTable checkedpairs;

int lshValidPairs = 0;

void genCandbySig();

// ==================== Core Minhash Function ====================
void load_computeSig() {
	printf("Load and Compute Sig ...\n");

	memset(sig, 0xff, sizeof(sig));
	for(int i=0 ; i<USERCOUNT ; i++)
		for(int j=HASHCOUNT>>1 ; j<HASHCOUNT ; j++)
			sig[i][j] = 0;

	HashFamily_init(&hashfamily, HASHCOUNT>>1);

	FILE * fin = fopen(pathdata, "r");
	for(int i=0 ; i<USERCOUNT ; i++) {
		int num;
		char feature[16];
		if(fscanf(fin, "%d%d", &i, &num) != 2) {
			fprintf(stderr, "read format error!\n");
			exit(1);
		}

		for(int j=0 ; j<num ; j++) {
			if(fscanf(fin, "%s", feature) != 1 || strlen(feature) != 11) {
				fprintf(stderr, "read format error!\n");
				exit(1);
			}

			int half = HASHCOUNT >> 1;
			for(int k=0 ; k<half ; k++) {
				unsigned int res = HashFamily_hash(&hashfamily, k, feature, 11);
				sig[i][k] = MIN(sig[i][k], res);
//				if(sig[i][k] > res) {
//					sig[i][k+half] = sig[i][k];
//					sig[i][k] = res;
//				}
//				else if(sig[i][k+half] > res)
//					sig[i][k+half] = res;
				sig[i][k+half] = MAX(sig[i][k+half], res);
			}
		}

		if(i && i % (USERCOUNT / 10) == 0)
			printf("Sig Complete:\t%.2lf%%\n", ((double)i) / USERCOUNT);
	}

	fclose(fin);
	HashFamily_destroy(&hashfamily);

//	genCandbySig();
}

struct BucketNode {
	uint32_t val;
	int id;
	int next;
};

#define BUCKETLEN (USERCOUNT * 3)
int bucket[BUCKETLEN];
struct BucketNode barray[USERCOUNT + 1];
int top = 0;
void Bucket_insert(uint32_t val, int id);
void Bucket_check(FILE * fout);
void Bucket_clear();

int ccnt = 0;
int cccnt = 0;


void computeLSH() {
	FILE * fout = fopen("./data/lsh.res", "w");
	HashFamily_init(&hashfamily, BANDCOUNT);
	HashTable_init(&checkedpairs, 500*USERCOUNT, 8);

	for(int i=0 ; i<BANDCOUNT ; i++) {
		for(int j=0 ; j<USERCOUNT ; j++) {

			char str[LINEOFROWS*4 + 1];
			str[LINEOFROWS*4] = 'k';
			for(int k=0 ; k<LINEOFROWS ; k++)
				memcpy(str+k*4, &sig[j][i*LINEOFROWS+k], 4);
			assert(str[LINEOFROWS*4] == 'k');

			uint32_t hashval = HashFamily_hash(&hashfamily, i, str, LINEOFROWS*4);
			Bucket_insert(hashval, j);
		}

		Bucket_check(fout);
		Bucket_clear();
	}
	HashFamily_destroy(&hashfamily);
	HashTable_destroy(&checkedpairs);
	fclose(fout);

	printf("LSH validPairs:\t%d\n", lshValidPairs);
	printf("%d %d\n", ccnt, cccnt);
}

// ==================== Bucket Function ====================

void Bucket_insert(uint32_t val, int id) {
	uint32_t index = val % BUCKETLEN;
	top++;
	assert(top <= USERCOUNT);
	barray[top].val = val;
	barray[top].id = id;
	barray[top].next = bucket[index];
	bucket[index] = top;
}

char isValidPairs(int a, int b) {
	int same = 0;
	for(int i=0 ; i<HASHCOUNT ; i++)
		if(sig[a][i] == sig[b][i])
			same++;

	if((double)same / HASHCOUNT > THRESHOLD)
		return 1;
	else
		return 0;
}

union HashStr {
	char str[8];
	struct M {
		int min;
		int max;
	}m;
};

void Bucket_check(FILE * fout) {

	for(int i=0 ; i<BUCKETLEN ; i++ )
		for(int p = bucket[i] ; p != 0 ; p = barray[p].next) 
			for(int np = barray[p].next ; np != 0 ; np = barray[np].next)
				if(barray[p].val == barray[np].val) {
					ccnt++;
					union HashStr hs;
					hs.m.min = MIN(barray[p].id, barray[np].id);
					hs.m.max = MAX(barray[p].id, barray[np].id);
					assert(sizeof(union HashStr) == 8);
					if(HashTable_insert(&checkedpairs, hs.str, 8) == 1) {
						cccnt++;
						if(isValidPairs(hs.m.min, hs.m.max)) {
							fprintf(fout, "%d\t%d\n", hs.m.min, hs.m.max);
							lshValidPairs++;
						}
					}
				}

}

void Bucket_clear() {
	assert(top == USERCOUNT);
	top = 0;
	memset(bucket, 0, sizeof(bucket));
	memset(barray, 0, sizeof(barray[0]));
}

// ==================== Output Function ====================

void genCandbySig() {
	printf("Generating Candidate by Sig ...\n");
	int validpair = 0 ;

	FILE * fout = fopen("./data/sig.res", "w");
	for(int a=0 ; a<USERCOUNT ; a++)
		for(int b=a+1 ; b<USERCOUNT ; b++) {
			if(isValidPairs(a, b)) {
				fprintf(fout, "%d\t%d\n", a, b);
				validpair++;
			}
		}
	fclose(fout);
	printf("Complete. Valid pair by Sig:\t%d\n", validpair);
}
