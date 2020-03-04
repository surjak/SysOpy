#include <stdio.h>
#include <stdlib.h>
char **table = NULL;
unsigned int initializated = 0;
unsigned int table_size = 0;
char *tmp_file = NULL;

int init_table(unsigned int size)
{
    if (initializated == 1)
    {
        fprintf(stderr, "already initializated");
        return -1;
    }

    table = calloc(size, sizeof(char *));
    table_size = size;
    initializated = 1;
    tmp_file = "tmp.txt";
    return 0;
}

int diff_files(char *file1, char *file2)
{
    char buf[256];
    snprintf(buf, sizeof buf, "diff %s %s > %s", file1, file2, tmp_file);
    int find_status = system(buf);
    if (find_status != 0)
        return find_status;

    return 0;
}

int get_file_size(FILE *fp)
{
    fseek(fp, 0L, SEEK_END);
    int sz = ftell(fp);
    rewind(fp);
    return sz;
}

int load_file(char **buffer, char *file_path)
{
    FILE *f = fopen(file_path, "rb");
    if (!f)
    {
        fprintf(stderr, "Problem with reading file");
        return -1;
    }
    long file_size = get_file_size(f);
    *buffer = calloc(file_size, sizeof(char));
    fread(*buffer, sizeof(char), file_size, f);
    fclose(f);
    return 0;
}

int file_to_array(char *file_name)
{
    for (int i = 0; i < table_size; i++)
    {
        if (table[i] == NULL)
        {
            int load_status = load_file(&table[i], file_name);
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

char *get_block(int idx)
{
    return table[idx];
}

int delete_block(int idx)
{
    free(table[idx]);
    table[idx] = NULL;
    return 0;
}

void delete_array()
{
    for (int i = 0; i < table_size; ++i)
    {
        delete_block(i);
    }
    free(table);
}

int main()
{
    // init_table(10);
    // FILE *f = NULL;
    // f = fopen("main.c", "r");
    // int a = get_file_size(f);
    // fclose(f);
    // printf("%d\n", a);
    // diff_files("a.txt", "b.txt");
    init_table(10);
    diff_files("a.txt", "b.txt");
    tmp_to_array();
    for (int i = 0; i < 10; ++i)
    {
        int idx = i;
        printf("idx: %d\n", idx);
        printf("block: %s\n", get_block(idx));
        delete_block(idx);
    }

    return 0;
}