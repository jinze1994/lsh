rules = -std=c99 -O3 -Wall -Wextra -Wshadow -m64

all: test

test: minhash.o hashfactory.o test.c
	gcc -o test test.c minhash.o hashfactory.o $(rules)

gen: hashfactory.o gen.c
	gcc -o gen gen.c hashfactory.o $(rules)

naive: hashfactory.o naive.c
	gcc -o naive naive.c hashfactory.o $(rules)

minhash.o: minhash.h minhash.c
	gcc -c minhash.c $(rules)

hashfactory.o: hashfactory.h hashfactory.c
	gcc -c hashfactory.c $(rules)

clean:
	rm -rf test gen naive *.o *~
