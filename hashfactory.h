#ifndef HASHFACTORY_H
#define HASHFACTORY_H

#include <stdint.h>

// ==================== Hash Table ====================
// Design for String hash. And the upper limit of capacity is estimable.
struct HashNode {
	int strend;
	int link;
};

struct HashTable {
	int bucketlen;
	int * bucket;

	int arraylen;
	struct HashNode * array;
	int top;
	int iterator;

	int stringpoollen;
	char * stringpool;
};

void HashTable_init(struct HashTable * phashtable, int max_capacity, int string_avg_len);

char HashTable_insert(struct HashTable * phashtable, const void * key, int len);

char * HashTable_iterator(struct HashTable * phashtable, int* plen);

void HashTable_reset(struct HashTable * phashtable);

char HashTable_exist(struct HashTable * phashtable, const void * key, int len);

void HashTable_clear(struct HashTable * ph);

void HashTable_destroy(struct HashTable * phashtable);

void HashTable_print(struct HashTable * phashtable);	// for Debug

// ==================== Hash Function ====================

uint32_t Murmur3Hash(const void * key, int len, uint32_t seed);

uint32_t DJBHash(const void * key, int len);


// ==================== Hash Family ====================
struct HashFamily {
	uint32_t *seed;
	int num;
};

void HashFamily_init(struct HashFamily * phashfamily, int num);

uint32_t HashFamily_hash(struct HashFamily * phashfamily, int id, const void * str, int len);

void HashFamily_destroy(struct HashFamily * phashfamily);

#endif
