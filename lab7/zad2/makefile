CC=gcc -Wall

all: build test clean

build:
	$(CC) -o main main.c -lpthread -lrt
	$(CC) -o worker1 worker_1.c -lpthread -lrt
	$(CC) -o worker2 worker_2.c -lpthread -lrt
	$(CC) -o worker3 worker_3.c -lpthread -lrt

test:
	./main

clean:
	rm -f main worker1 worker2 worker3