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
