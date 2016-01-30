all: cbsolve

cbsolve: cbsolve.c
	$(CC) -O2 -Wall -ggdb cbsolve.c -o cbsolve

clean:
	rm -f cbsolve

test: cbsolve
	./cbsolve < cb1-ex-56.txt
