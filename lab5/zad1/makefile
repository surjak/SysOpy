CC = gcc
FLAGS = -std=c99 -Wall

all:
	make clean
	make pipes
	make popen

pipes:
	$(CC) $(FLAGS) -o pipes pipes.c 
	./pipes file.txt

popen:
	$(CC) $(FLAGS) -o popen popen.c 
	./popen file.txt
clean:
	rm pipes popen