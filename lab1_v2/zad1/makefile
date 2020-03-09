CC = gcc
FLAGS = -Wall
LIB_NAME = lib_diff
SOURCE_NAME = lib_diff


static:
	$(CC) $(FLAGS) -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	ar rcs $(LIB_NAME).a $(LIB_NAME).o
	make clean

shared:
	$(CC) -fPIC $(FLAGS) -c $(SOURCE_NAME).c -o $(LIB_NAME).o
	$(CC) -fPIC $(FLAGS) -shared -o $(LIB_NAME).so $(LIB_NAME).o
	make clean

clean:
	rm -f *.o

clean_all:
	rm main tmp.txt $(LIB_NAME).a $(LIB_NAME).so