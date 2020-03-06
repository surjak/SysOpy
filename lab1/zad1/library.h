#ifndef LAB1_LIBRARY_H
#define LAB1_LIBRARY_H
#pragma once

int init_table(unsigned int size);
void define_pair_sequence(char *sequence);
void diff_all_elements_from_sequence_and_save_to_array();
int get_operations_count(int idx);
int tmp_to_array();
int delete_block(int idx);
void delete_array();
void delete_operation_from_block(int block_idx, int operation_idx);
char *get_block(int idx);

#endif //LAB1_LIBRARY_H