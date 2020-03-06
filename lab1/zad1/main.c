#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    create_table(100);

    define_pair_sequence("a.txt b.txt c.txt d.txt e.txt f.txt");

    compare_pairs();
    compare_pairs();
    compare_pairs();
    compare_pairs();
    compare_pairs();
    compare_pairs();
    compare_pairs();
    compare_pairs();
    compare_pairs();
    compare_pairs();
    // remove_operation(25, 0);
    remove_operation(26, 0);
    remove_operation(29, 0);
    delete_block(26);
    // compare_pairs();

    for (int i = 0; i < 100; ++i)
    {
        int idx = i;
        printf("idx: %d\n", idx);
        printf("block: %s\n\n", get_block(idx));
        delete_block(idx);
    }
    return 0;
}