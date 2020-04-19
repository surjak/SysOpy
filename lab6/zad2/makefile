CC=gcc -Wall

all: build test clean

build: client.c server.c
	$(CC) -o client client.c -lrt
	$(CC) -o server server.c -lrt

test: server
	./server

clean:
	rm -f client server