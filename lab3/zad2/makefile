CC = gcc -Wall -std=c11 -ggdb

all: 
	make test

clean:
	rm -f -r .tmp files pom matrix lista

helper:
	rm -f pom
	$(CC) help.c -o pom

test:
	make clean
	$(CC) help.c -o pom 
	$(CC) matrix.c -o matrix
	./pom create 7 14 5
	./matrix lista 6 3 1
	./pom check lista

paste:
	make clean
	$(CC) help.c -o pom 
	$(CC) matrix.c -o matrix
	./pom create 7 14 5
	./matrix lista 6 3 2

flock:
	make clean
	$(CC) help.c -o pom 
	$(CC) matrix.c -o matrix
	./pom create 7 14 5
	./matrix lista 6 3 1
