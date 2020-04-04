#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

long get_file_size(FILE *f)
{
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return file_size;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "wrong arguments, usage: [file]\n");
        exit(1);
    }
    char *file_path = argv[1];
    FILE *f = fopen(file_path, "r");
    if (f == NULL)
    {
        fprintf(stderr, "can't find or open file %s\n", file_path);
        exit(1);
    }
    long file_size = get_file_size(f);
    char *buffer = malloc(file_size + 1);
    if (fread(buffer, 1, file_size, f) != file_size)
    {
        fprintf(stderr, "cant read from file %s\n", file_path);
        exit(1);
    }
    fclose(f);
    char *lines_tmp = buffer;
    char *line = strtok_r(buffer, "\n", &lines_tmp);
    while (line != NULL)
    {
        FILE *popen_file = popen(line, "w");
        pclose(popen_file);
        line = strtok_r(NULL, "\n", &lines_tmp);
    }
    free(buffer);
    return 0;
}