CC = gcc
FLAGS = -std=c99 -Wall

.PHONY: server client

server:
	$(CC) $(FLAGS) server.c keygen.c que.c utils.c -o server

client:
	$(CC) $(FLAGS) client.c keygen.c que.c utils.c -o client

clean:
	rm server client