CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g -pthread

all: 
	make clean
	make server
	make client
	./server 6666 socket

client:
	$(CC) $(CFLAGS) -o client client.c

server:
	$(CC) $(CFLAGS) -o server server.c


client1:
	./client Mateusz local socket

client2:
	./client Tomek local socket

client1_host:
	./client Mateusz xxx 6666

client2_host:
	./client Tomek xxx 6666

clean:
	rm -f $(obj) client server socket