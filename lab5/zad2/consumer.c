#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Wrong arguments: [pipe_path] [file_path] [character count]");
        exit(1);
    }
    char *pipe_path = argv[1];
    char *file_path = argv[2];
    int N = atoi(argv[3]);
    FILE *write_file = fopen(file_path, "w");
    if (write_file == NULL)
    {
        fprintf(stderr, "Can't open or read file: %s", file_path);
        exit(1);
    }
    FILE *pipe_file = fopen(pipe_path, "r");
    if (pipe_file == NULL)
    {
        fprintf(stderr, "Can't open or read file: %s", pipe_path);
        exit(1);
    }
    char buffer[N];
    while (fgets(buffer, N, pipe_file) != NULL)
    {
        fprintf(write_file, buffer, strlen(buffer));
    }
    fclose(pipe_file);
    fclose(write_file);
    return 0;
}