CC=gcc -Wall

all: build test clean

build:
	$(CC) -o main main.c
	$(CC) -o worker_1 worker_1.c shared.c
	$(CC) -o worker_2 worker_2.c shared.c
	$(CC) -o worker_3 worker_3.c shared.c

test:
	./main

clean:
	rm -f main worker_1 worker_2 worker_3