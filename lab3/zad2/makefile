CC = gcc -Wall -std=c11 -ggdb

all: 
	make test

clean:
	rm -f -r .tmp files help matrix lista

helper:
	rm -f help
	$(CC) help.c -o help

test:
	make clean
	$(CC) help.c -o help 
	$(CC) matrix.c -o matrix
	./help create 7 14 5
	./matrix lista 6 3 1
	./help check lista

paste:
	make clean
	$(CC) help.c -o help 
	$(CC) matrix.c -o matrix
	./help create 7 14 5
	./matrix lista 6 3 2

flock:
	make clean
	$(CC) help.c -o help 
	$(CC) matrix.c -o matrix
	./help create 7 14 5
	./matrix lista 6 3 1
