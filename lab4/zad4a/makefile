CC = gcc
FLAGS = -Wall -std=c99 

all:
	make catcher
	make sender
catcher:
	$(CC) $(FLAGS) catcher.c -o catcher

sender:
	$(CC) $(FLAGS) sender.c -o sender

clean:
	rm -rf sender catcher