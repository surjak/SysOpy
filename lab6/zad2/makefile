CC = gcc -Wall 

all:
	gcc server.c types.c -o server -lrt
	gcc client.c types.c -o client -lrt

clean:
	rm -f client server *.o 