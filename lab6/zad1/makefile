CC = gcc
FLAGS = -std=c99 -Wall

all:
	make clean
	make server
	make client
make r:
	make server
	make client
server:
	$(CC) $(FLAGS) server.c keygen.c que.c utils.c -o server

client:
	$(CC) $(FLAGS) client.c keygen.c que.c utils.c -o client

clean:
	rm server client