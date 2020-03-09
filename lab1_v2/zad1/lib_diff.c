#include <stdlib.h>
#include <stdio.h>
#include "lib_diff.h"
#include <string.h>

char *tmp_file = "tmp.txt";
char *files_sequence = NULL;
Diff_handler *diff_handler;
int create_table(unsigned int size)
{
    diff_handler = calloc(1, sizeof(diff_handler));
    diff_handler->size = size;
    diff_handler->block_array = calloc(size, sizeof(Operations *));
    return 1;
}

int get_file_size(FILE *fp)
{
    fseek(fp, 0L, SEEK_END);
    int sz = ftell(fp);
    rewind(fp);
    return sz;
}

void define_pair_sequence(char *files)
{
    files_sequence = files;
}

int diff_files(char *file1, char *file2)
{
    char buf[256];
    snprintf(buf, sizeof buf, "diff %s %s > %s", file1, file2, tmp_file);
    int find_status = system(buf);
    if (find_status != 0)
    {

        return find_status;
    }

    return 0;
}

void load_buffer_into_array(char *buff, Operation ***tab, int size)
{
    char *stack[size];

    char *p = buff;

    int i = 0;
    stack[i] = p;

    while (*p != '\0')
    {

        if (*p == '\n' && (*(++p) > '0' && *(p) < '9'))
        {
            i++;
            stack[i] = p;
        }
        p++;
    }

    for (int j = 0; j < size; j++)
    {
        if (j == size - 1)
        {
            (*tab)[j]->content = calloc(strlen(stack[j]) + 1, sizeof(char));
            memcpy((*tab)[j]->content, stack[j], strlen(stack[j]));
            return;
        }
        (*tab)[j]->content = calloc(strlen(stack[j]) - strlen(stack[j + 1]) + 1, sizeof(char));
        memcpy((*tab)[j]->content, stack[j], strlen(stack[j]) - strlen(stack[j + 1]));
    }
}
int get_count_of_operations_in_buffer(char *content)
{
    char *p;
    int counter = 0;
    p = content;
    int flag = 0;

    while (*p != '\0')
    {
        flag = 1;
        if (*p == '\n' && (*(++p) > '0' && *(p) < '9'))
            counter++;
        p++;
    }
    int val_to_ret = 0;
    if (flag == 1)
        val_to_ret = counter + 1;
    return val_to_ret;
}

int load_file(Operations **buffer)
{
    char *buf = NULL;
    FILE *f = fopen(tmp_file, "rb");
    if (!f)
    {
        fprintf(stderr, "Problem with reading file");
        return -1;
    }
    long file_size = get_file_size(f);
    buf = calloc(file_size, sizeof(char));
    fread(buf, sizeof(char), file_size, f);
    fclose(f);

    int count = get_count_of_operations_in_buffer(buf);
    (*buffer) = calloc(1, sizeof(Operations));
    (*buffer)->count = count;
    (*buffer)->operations = calloc(count, sizeof(Operation *));
    for (int i = 0; i < count; i++)
    {
        (*buffer)->operations[i] = calloc(1, sizeof(Operation));
    }
    load_buffer_into_array(buf, &((*buffer)->operations), count);
    return 0;
}

int file_to_array(char *file_name)
{
    for (int i = 0; i < diff_handler->size; i++)
    {
        if (diff_handler->block_array[i] == NULL)
        {
            int load_status = load_file(&diff_handler->block_array[i]);
            if (load_status < 0)
            {
                fprintf(stderr, "can't load into array\n");
                return load_status;
            }
            return i;
        }
    }
    fprintf(stderr, "can't load into array, array is full\n");
    return -1;
}

int tmp_to_array()
{
    return file_to_array(tmp_file);
}

void compare_pair(char *pair)
{
    char *string = strdup(pair);

    char *f1 = strtok(string, ":");
    char *f2 = strtok(NULL, "");

    diff_files(f1, f2);
    tmp_to_array();
}

void compare_pairs()
{

    char *string = strdup(files_sequence);

    char *token = strtok(string, " ");
    int count = 0;
    while (token != NULL)
    {
        token = strtok(NULL, " ");
        count++;
    }

    char *pairs[count];
    int i = 0;
    char *string_2 = strdup(files_sequence);

    char *token_2 = strtok(string_2, " ");
    while (token_2 != NULL)
    {
        pairs[i] = token_2;
        i++;
        token_2 = strtok(NULL, " ");
    }

    for (int j = 0; j < count; j++)
    {
        compare_pair(pairs[j]);
    }
}

int get_operations_count(int idx)
{
    if (diff_handler->block_array[idx] == NULL)
        return 0;
    return diff_handler->block_array[idx]->count;
}

void delete_block(int idx)
{
    if (diff_handler->block_array[idx] == NULL)
        return;
    for (int i = 0; i < diff_handler->block_array[idx]->count; i++)
    {
        free(diff_handler->block_array[idx]->operations[i]);
        diff_handler->block_array[idx]->operations[i] = NULL;
    }
    free(diff_handler->block_array[idx]->operations);
    diff_handler->block_array[idx]->operations = NULL;
    free(diff_handler->block_array[idx]);
    diff_handler->block_array[idx] = NULL;
}

void remove_operation(int block_idx, int operation_idx)
{
    if (diff_handler->block_array[block_idx] == NULL)
    {
        fprintf(stderr, "index in array doesn't exist\n");
        return;
    }

    if (diff_handler->block_array[block_idx]->operations[operation_idx] == NULL)
    {
        fprintf(stderr, "index in operation array doesn't exist\n");
        return;
    }

    free(diff_handler->block_array[block_idx]->operations[operation_idx]);
    diff_handler->block_array[block_idx]->operations[operation_idx] = NULL;
    int count = diff_handler->block_array[block_idx]->count;
    if (count - 1 <= 0)
    {
        delete_block(block_idx);
    }
    else
    {
        diff_handler->block_array[block_idx]->count = count - 1;
    }
}

Operations *get_block(int idx)
{
    if (diff_handler->block_array[idx] == NULL)
    {
        fprintf(stderr, "index doesn't exist\n");
    }

    return diff_handler->block_array[idx];
}

char *get_operation(int block_idx, int operation_idx)
{
    if (diff_handler->block_array[block_idx] == NULL)
    {
        fprintf(stderr, "index in array doesn't exist\n");
    }
    if (diff_handler->block_array[block_idx]->operations[operation_idx] == NULL)
    {
        fprintf(stderr, "index in operation array doesn't exist\n");
    }
    return diff_handler->block_array[block_idx]->operations[operation_idx]->content;
}

void delete_array()
{
    for (int i = 0; i < diff_handler->size; i++)
    {
        delete_block(i);
    }
    free(diff_handler->block_array);
    free(diff_handler);
}

// int main()
// {
//     define_pair_sequence("txt/a.txt:txt/b.txt txt/c.txt:txt/d.txt txt/a.txt:txt/c.txt txt/e.txt:txt/f.txt txt/f.txt:txt/c.txt");
//     create_table(20);
//     compare_pairs();
//     tmp_to_array();

//     // printf("%s", diff_handler->block_array[0]->operations[2]->content);
//     delete_block(0);
//     // printf("%s", diff_handler->block_array[2]->operations[0]->content);
//     // compare_pairs();
//     tmp_to_array();
//     remove_operation(0, 0);
//     // remove_operation(0, 0);
//     // remove_operation(0, 23);
//     // remove_operation(0, 3);
//     // tmp_to_array();

//     compare_pairs();
//     // printf("%d\n", get_operations_count(0));

//     Operations *ope = get_block(0);
//     for (int i = 0; i < ope->count; i++)
//     {
//         printf("%d: \n%s\n\n", i, ope->operations[i]->content);
//     }

//     // printf("\n\n\n\n\n%s\n\n", get_operation(0, 1));
//     delete_array();
// }