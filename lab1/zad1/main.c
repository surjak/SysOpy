#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    init_table(100);

    define_pair_sequence("a.txt b.txt c.txt d.txt e.txt f.txt");

    diff_all_elements_from_sequence_and_save_to_array();
    diff_all_elements_from_sequence_and_save_to_array();
    diff_all_elements_from_sequence_and_save_to_array();
    diff_all_elements_from_sequence_and_save_to_array();
    diff_all_elements_from_sequence_and_save_to_array();
    diff_all_elements_from_sequence_and_save_to_array();
    diff_all_elements_from_sequence_and_save_to_array();
    diff_all_elements_from_sequence_and_save_to_array();
    diff_all_elements_from_sequence_and_save_to_array();
    diff_all_elements_from_sequence_and_save_to_array();
    // delete_operation_from_block(25, 0);
    delete_operation_from_block(26, 0);
    delete_operation_from_block(29, 0);
    delete_block(26);
    // diff_all_elements_from_sequence_and_save_to_array();

    for (int i = 0; i < 100; ++i)
    {
        int idx = i;
        printf("idx: %d\n", idx);
        printf("block: %s\n\n", get_block(idx));
        delete_block(idx);
    }
    return 0;
}