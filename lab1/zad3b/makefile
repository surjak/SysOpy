CC = gcc
FLAGS = -std=c99 -Wall
LIB_NAME = lib_diff
LIB_PATH = ../zad1
TEST_PATH = ../zad2
TEST_SOURCE_NAME = main.c
REPORT_FILE = raport3b.txt
O = O1

static:
	$(CC) $(FLAGS) $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main -$O -I$(LIB_PATH) $(LIB_PATH)/$(LIB_NAME).a 

shared:
	$(CC) $(FLAGS) $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main -$O -I$(LIB_PATH) -L$(LIB_PATH) -l_diff 

dynamic:
	$(CC) $(FLAGS) $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main -$O -I$(LIB_PATH)  -ldl -D DYNAMIC	

test:
	LD_LIBRARY_PATH=$(LIB_PATH) ./main $(shell cat args.txt)

test_all:
	rm -f $(REPORT_FILE)
	rm -f raport.txt

	printf "static O1:\n" > raport.txt
	make static O=O1
	make test
	printf "static O2:\n" >> raport.txt
	make static O=O2
	make test
	printf "static: O3\n" >> raport.txt
	make static O=O3
	make test
	printf "static: Ofast\n" >> raport.txt
	make static O=Ofast
	make test
	printf "static: Os\n" >> raport.txt
	make static O=Os
	make test
	printf "static: O0\n" >> raport.txt
	make static O=O0
	make test
	mv raport.txt $(REPORT_FILE)