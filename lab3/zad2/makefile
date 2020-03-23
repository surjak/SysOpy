CC=gcc -Wall

all: build generate run

build: generate.c matrix.c
	$(CC) -o generate generate.c
	$(CC) -o matrix matrix.c -lm


generate: generate
	mkdir tests
	./generate 10 5 50

run: matrix
	./matrix list 4 1 1



clean:
	rm -rf tests generate matrix list