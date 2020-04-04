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

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "wrong arguments, usage: [file]\n");
        exit(1);
    }
    char *file_path = argv[1];

    char line[200];
    sprintf(line, "cat %s | sort", file_path);
    FILE *popen_file = popen(line, "w");
    pclose(popen_file);

    return 0;
}