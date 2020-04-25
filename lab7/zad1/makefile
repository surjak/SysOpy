CC=gcc -Wall

all: build test clean

build: main.c worker_1.c worker_2.c worker_3.c
	$(CC) -o main main.c
	$(CC) -o worker1 worker_1.c
	$(CC) -o worker2 worker_2.c
	$(CC) -o worker3 worker_3.c

test: main worker1 worker2 worker3
	./main

clean:
	rm -f main worker1 worker2 worker3