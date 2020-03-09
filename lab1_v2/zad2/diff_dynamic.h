#ifndef DIFF_DYN_H
#define DIFF_DYN_H
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

static void *handle = NULL;

char *(*_get_operation)(int block_idx, int operation_idx);
int (*_create_table)(unsigned int size);
void (*_define_pair_sequence)(char *sequence);
void (*_compare_pairs)();
int (*_get_operations_count)(int idx);
int (*_tmp_to_array)();
void (*_delete_block)(int idx);
void (*_delete_array)();
void (*_remove_operation)(int block_idx, int operation_idx);

void init_dynamic_library()
{
    handle = dlopen("lib_diff.so", RTLD_NOW);
    if (handle == NULL)
    {
        fprintf(stderr, "Error dynamic library open");
        return;
    }
    _get_operation = dlsym(handle, "get_operation");
    _create_table = dlsym(handle, "create_table");
    _define_pair_sequence = dlsym(handle, "define_pair_sequence");
    _compare_pairs = dlsym(handle, "compare_pairs");
    _get_operations_count = dlsym(handle, "get_operations_count");
    _tmp_to_array = dlsym(handle, "tmp_to_array");
    _delete_block = dlsym(handle, "delete_block");
    _delete_array = dlsym(handle, "delete_array");
    _remove_operation = dlsym(handle, "remove_operation");

    char *error;
    if ((error = dlerror()) != NULL)
    {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
}

int create_table(unsigned int size)
{
    return (*_create_table)(size);
}
void define_pair_sequence(char *sequence)
{
    return (*_define_pair_sequence)(sequence);
}
void compare_pairs()
{
    return (*_compare_pairs)();
}
int get_operations_count(int idx)
{
    return (*_get_operations_count)(idx);
}
int tmp_to_array()
{
    return (*_tmp_to_array)();
}
void delete_block(int idx)
{
    return (*_delete_block)(idx);
}
void delete_array()
{
    return (*_delete_array)();
}
void remove_operation(int block_idx, int operation_idx)
{
    return (*_remove_operation)(block_idx, operation_idx);
}
char *get_operation(int block_idx, int operation_idx)
{
    return (*_get_operation)(block_idx, operation_idx);
}

#endif