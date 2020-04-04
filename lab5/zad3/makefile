all:
	gcc -Wall -std=c99 -o producer producer.c
	gcc -Wall -std=c99 -o consumer consumer.c
	gcc -Wall -std=c99 -o main main.c
	./main
	make clean
prod:
	./producer fif producer_data/a 5
cons:
	./consumer fif consumer_data/a 5
clean:
	rm fif producer main consumer