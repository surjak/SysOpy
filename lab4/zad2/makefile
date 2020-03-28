all:
	make normal
	make clean

exec:
	gcc -Wall -std=c99 -o  main main.c
	gcc -Wall -std=c99  -o exec exec.c
	./main ignore exec
	./main pending exec
	./main mask exec
	./main handler exec

normal:
	gcc -Wall -std=c99  -o main main.c
	./main ignore n
	./main pending n
	./main mask n
	./main handler n

clean:
	rm -rf main exec