#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "utils.h"

void split_line(char *line, char *output[], int *output_len)
{
    char line_cpy[256];
    strcpy(line_cpy, line);

    char *line_tmp = line_cpy;
    char *arg = strtok_r(line_cpy, " ", &line_tmp);

    int arg_cnt = 0;

    while (arg != NULL && arg[0] != EOF)
    {
        output[arg_cnt++] = strdup(arg);
        arg = strtok_r(NULL, " ", &line_tmp);
    }
    *output_len = arg_cnt;
}

void separate_command(char *line, char *command, char *rest)
{
    char line_cpy[256];
    strcpy(line_cpy, line);

    char *line_tmp = line_cpy;
    char *cmd = strtok_r(line_cpy, " \n", &line_tmp);

    char *txt = strtok_r(NULL, "\n", &line_tmp);
    if (txt == NULL)
    {
        rest[0] = '\0';
    }
    else
    {
        strcpy(rest, txt);
    }
    if (cmd == NULL)
    {
        command[0] = '\0';
    }
    else
    {
        strcpy(command, cmd);
    }
}

void get_lines_from_file(char *file_path, char *lines[], int *num_lines)
{

    FILE *f = fopen(file_path, "r");
    if (f == NULL)
    {
        fprintf(stderr, "cant open file %s\n", file_path);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = malloc(fsize + 1);
    if (fread(buffer, 1, fsize, f) != fsize)
    {
        fprintf(stderr, "cant read from file %s\n", file_path);
        exit(1);
    }

    fclose(f);

    char *buff_tmp = buffer;
    char *line = strtok_r(buffer, "\n", &buff_tmp);

    int lines_count = 0;
    while (line != 0 && line[0] != EOF)
    {
        lines[lines_count++] = line;
        line = strtok_r(NULL, "\n", &buff_tmp);
    }

    free(buffer);
    *num_lines = lines_count;
}