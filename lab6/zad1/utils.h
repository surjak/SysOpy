#ifndef UTILS_H
#define UTILS_H

void split_line(char *line, char *output[], int *output_len);
void separate_command(char *line, char *command, char *rest);

#endif