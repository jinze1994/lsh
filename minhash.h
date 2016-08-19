#ifndef MINHASH_H
#define MINHASH_H

#include <stdint.h>

#define USERCOUNT 10000

#define BANDCOUNT 32
#define LINEOFROWS 20
#define HASHCOUNT (BANDCOUNT * LINEOFROWS)

#define THRESHOLD 0.83

void load_computeSig();

void genCandbySig();

void computeLSH();


#endif
