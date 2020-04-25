CC=gcc -Wall

all: build test clean

build: main.c worker_1.c worker_2.c worker_3.c
	$(CC) -o main main.c
	$(CC) -o worker_1 worker_1.c
	$(CC) -o worker_2 worker_2.c
	$(CC) -o worker_3 worker_3.c

test: main worker_1 worker_2 worker_3
	./main

clean:
	rm -f main worker_1 worker_2 worker_3