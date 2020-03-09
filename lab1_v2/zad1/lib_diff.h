#ifndef LAB1_LIBRARY_H
#define LAB1_LIBRARY_H
#pragma once
#include <stdio.h>
#include <stdlib.h>
typedef struct
{
    char *content;
} Operation;

typedef struct
{
    int count;
    Operation **operations;
} Operations;

typedef struct
{
    int size;
    Operations **block_array;
} Diff_handler;

int create_table(unsigned int size);
void define_pair_sequence(char *files);
void compare_pairs();
int get_operations_count(int idx);
int tmp_to_array();
void delete_block(int idx);
void delete_array();
void remove_operation(int block_idx, int operation_idx);
char *get_operation(int block_idx, int operation_idx);

#endif //LAB1_LIBRARY_H