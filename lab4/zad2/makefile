all:
	make normal
	make clean

exec:
	gcc -o main main.c
	gcc -o exec exec.c
	./main ignore exec
	./main pending exec
	./main mask exec
	./main handler exec

normal:
	gcc -o main main.c
	./main ignore n
	./main pending n
	./main mask n
	./main handler n

clean:
	rm -rf main exec