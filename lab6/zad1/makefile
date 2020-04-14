
CC = gcc
FLAGS = -std=c99 -Wall

all:
	make server
	./server
	make clean
server:
	$(CC) $(FLAGS) server.c keygen.c queue.c -o server

client:
	$(CC) $(FLAGS) client.c keygen.c queue.c -o client

clean:
	rm server