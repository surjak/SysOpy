#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    create_table(20);

    define_pair_sequence("a.txt b.txt c.txt d.txt e.txt f.txt");

    compare_pairs();
    compare_pairs();
    compare_pairs();
    compare_pairs();
    compare_pairs();

    // remove_operation(25, 0);
    // remove_operation(26, 0);
    remove_operation(12, 1);
    delete_block(14);
    // compare_pairs();

    for (int i = 0; i < 20; ++i)
    {
        int idx = i;
        printf("idx: %d\n", idx);
        printf("block: %s\n\n", get_block(idx));
        delete_block(idx);
    }
    return 0;
}