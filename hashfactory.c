#include "hashfactory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

// ==================== Hash Table ====================

void HashTable_init(struct HashTable * ph, int max_capacity, int string_avg_len) {
	ph->bucketlen = max_capacity * 3;
	ph->bucket = calloc(ph->bucketlen, sizeof(int));

	ph->arraylen = max_capacity + 1;
	ph->array = malloc(ph->arraylen * sizeof(struct HashNode));
	memset(ph->array, 0, sizeof(ph->array[0]));
	ph->top = 0;
	ph->iterator = 0;

	ph->stringpoollen = max_capacity * string_avg_len;
	ph->stringpool = malloc(ph->stringpoollen);
}

char HashTable_insert(struct HashTable * ph, const void * key, int len) {
	assert(len > 0);
	uint32_t hashval = Murmur3Hash(key, len, 0xDEEDBEEF);
	// uint32_t hashval = 0;
	int index = hashval % ph->bucketlen;
	for(int i=ph->bucket[index] ; i!=0 ; i=ph->array[i].link) {
		int off = ph->array[i-1].strend;
		int old_len = ph->array[i].strend - off;
		if(old_len != len)
			continue;
		const char * old_start = ph->stringpool + off;
		if(memcmp(old_start, key, len) == 0)
			return 0;
	}

	int lastoff = ph->array[ph->top].strend;
	if(lastoff + len > ph->stringpoollen) {
		fprintf(stderr, "String pool capacity overflow\n");
		exit(1);
	}
	ph->top++;
	if(ph->top >= ph->arraylen) {
		fprintf(stderr, "Hashtable capacity overflow\n");
		exit(1);
	}

	ph->array[ph->top].strend = lastoff + len;
	memcpy((ph->stringpool)+lastoff, key, len);

	ph->array[ph->top].link = ph->bucket[index];
	ph->bucket[index] = ph->top;
	return 1;
}

char * HashTable_iterator(struct HashTable * ph, int* plen) {
	if(ph->iterator >= ph->top) {
		*plen = 0;
		return NULL;
	}
	int off = (ph->array[ph->iterator]).strend;
	char * pstr = ph->stringpool + off;
	ph->iterator++;
	*plen = (ph->array[ph->iterator]).strend - off;
	return pstr;
}

void HashTable_reset(struct HashTable * ph) {
	ph->iterator = 0;
}

char HashTable_exist(struct HashTable * ph, const void * key, int len) {
	uint32_t hashval = Murmur3Hash(key, len, 0xDEEDBEEF);
	// uint32_t hashval = 0;
	int index = hashval % ph->bucketlen;
	for(int i=ph->bucket[index] ; i!=0 ; i=ph->array[i].link) {
		int off = ph->array[i-1].strend;
		int old_len = ph->array[i].strend - off;
		if(old_len != len)
			continue;
		const char * old_start = ph->stringpool + off;
		if(memcmp(old_start, key, len) == 0)
			return 1;
	}
	return 0;
}

void HashTable_clear(struct HashTable * ph) {
	memset(ph->bucket, 0, ph->bucketlen * sizeof(int));
	memset(ph->array, 0, sizeof(ph->array[0]));
	ph->top = 0;
	ph->iterator = 0;
}

void HashTable_destroy(struct HashTable * ph) {
	free(ph->bucket);
	free(ph->array);
	free(ph->stringpool);
	memset(ph, 0, sizeof(struct HashTable));
}

void HashTable_print(struct HashTable * ph) {
	printf("bucketlen: %d\narraylen: %d\nstringpoollen: %d\nnowtop: %d\nnowstringlen: %d\n", 
		ph->bucketlen, ph->arraylen, ph->stringpoollen, ph->top, ph->array[ph->top].strend);
	for(int i=1 ; i<=ph->top ; i++) {
		printf("\t%d\tstrend:%d\tlink: %d\t", i, ph->array[i].strend, ph->array[i].link);
		for(int j=ph->array[i-1].strend ; j<ph->array[i].strend ; j++)
			putchar(ph->stringpool[j]);
		printf("\n");
	}
}


// ==================== Hash Function ====================
// MurmurHash3_x86_32 
// Rewrite according to https://github.com/PeterScott/murmur3/blob/master/murmur3.c
// Authentic Vision: http://code.google.com/p/smhasher/source/browse/trunk/MurmurHash3.cpp?spec=svn136&r=136

#define ROTL32(x, r) ((x << r) | (x >> (32 - r)))

uint32_t Murmur3Hash(const void * key, int len, uint32_t seed) {

	const uint8_t * data = (const uint8_t*)key;
	const int nblocks = len / 4;

	uint32_t h1 = seed;
	uint32_t c1 = 0xcc9e2d51;
	uint32_t c2 = 0x1b873593;

	// body
	const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);
	for(int i = -nblocks; i; i++) {
		uint32_t k1 = blocks[i];

		k1 *= c1;
		k1 = ROTL32(k1,15);
		k1 *= c2;
		
		h1 ^= k1;
		h1 = ROTL32(h1,13); 
		h1 = h1*5+0xe6546b64;
	}

	// tail
	const uint8_t * tail = (const uint8_t*)(data + nblocks*4);
	uint32_t k1 = 0;
	switch(len & 3) {
	case 3: k1 ^= tail[2] << 16;
	case 2: k1 ^= tail[1] << 8;
	case 1: k1 ^= tail[0];
			k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
	};

	// finalization
	h1 ^= len;
	h1 ^= h1 >> 16;
	h1 *= 0x85ebca6b;
	h1 ^= h1 >> 13;
	h1 *= 0xc2b2ae35;
	h1 ^= h1 >> 16;
	return h1;
}

// DJB hash function
uint32_t DJBHash(const void * key, int len) {
	uint32_t hashval = 5381;
	uint8_t * data = (uint8_t*)key;
	const uint8_t * end = (const uint8_t*)(key + len);

	while(data != end)
		hashval += (hashval << 5) + (*data++);

	return hashval;
}

// ==================== Hash Family ====================

void HashFamily_init(struct HashFamily * phashfamily, int num) {
	assert(num > 0);
	phashfamily->num = num;
	phashfamily->seed = malloc(num * sizeof(uint32_t));

	for(int i=0 ; i<num ; i++)
		phashfamily->seed[i] = rand();
}

uint32_t HashFamily_hash(struct HashFamily * phashfamily, int id, const void * str, int len) {
	assert(id < phashfamily->num);
	assert(len > 0);
	return Murmur3Hash(str, len, phashfamily->seed[id]);
}

void HashFamily_destroy(struct HashFamily * phashfamily) {
	free(phashfamily->seed);
	memset(phashfamily, 0, sizeof(struct HashFamily));
}

