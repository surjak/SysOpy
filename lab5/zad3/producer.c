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

    FILE *read_file = fopen(file_path, "r");
    if (read_file == NULL)
    {
        fprintf(stderr, "Can't open or read file: %s", file_path);
        exit(1);
    }
    int pipe_file = open(pipe_path, O_WRONLY);
    if (pipe_file < 0)
    {
        fprintf(stderr, "Can't open file: %d", pipe_file);
        exit(1);
    }
    char buffer[N];
    srand(time(NULL));

    while (fgets(buffer, N, read_file) != NULL)
    {
        char message[N + 20];
        sprintf(message, "#%d#%s\n", getpid(), buffer);
        write(pipe_file, message, strlen(message));
        sleep(rand() % 2);
    }
    close(pipe_file);
    fclose(read_file);
    return 0;
}