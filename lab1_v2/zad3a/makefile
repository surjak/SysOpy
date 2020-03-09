
CC = gcc
FLAGS = -std=c99 -Wall
LIB_NAME = lib_diff
LIB_PATH = ../zad1
TEST_PATH = ../zad2
TEST_SOURCE_NAME = main.c
REPORT_FILE = raport3a.txt

static:
	$(CC) $(FLAGS) $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main -I$(LIB_PATH) $(LIB_PATH)/$(LIB_NAME).a 

shared:
	$(CC) $(FLAGS) $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main -I$(LIB_PATH) -L$(LIB_PATH) -l_diff 

dynamic:
	$(CC) $(FLAGS) $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main -I$(LIB_PATH)  -ldl -D DYNAMIC	

test:
	LD_LIBRARY_PATH=$(LIB_PATH) ./main $(shell cat args.txt)

test_all:
	printf "static:\n" > raport.txt
	make static
	make test
	printf "shared:\n" >> raport.txt
	make shared
	make test
	printf "dynamic:\n" >> raport.txt
	make dynamic
	make test
	mv raport.txt $(REPORT_FILE)