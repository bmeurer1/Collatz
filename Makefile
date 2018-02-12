all: collatzNumber collatzInterval

collatzNumber: collatzNumber.c
	gcc -o collatzNumber collatzNumber.c -O3 -lgmp

collatzInterval: collatzInterval.c
	gcc -o collatzInterval collatzInterval.c -O3 -lgmp -lpthread