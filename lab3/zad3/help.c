#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <linux/limits.h>
#include <stdbool.h>
#include "matrix_helper.c"

bool check_multiply_correctness(char *a_filename, char *b_filename, char *c_filename)
{
    matrix a = load_matrix(a_filename);
    matrix b = load_matrix(b_filename);
    matrix c = load_matrix(c_filename);
    matrix correct_matrix = multiply_matrices(a, b);
    if (correct_matrix.cols != c.cols || correct_matrix.rows != c.rows)
        return false;
    for (int i = 0; i < correct_matrix.rows; i++)
    {
        for (int j = 0; j < correct_matrix.cols; j++)
        {
            if (correct_matrix.values[i][j] != c.values[i][j])
                return false;
        }
    }
    free_matrix(&a);
    free_matrix(&b);
    free_matrix(&c);
    free_matrix(&correct_matrix);
    return true;
}

int main(int argc, char **argv)
{
    char *mode = calloc(10, sizeof(char));
    mode = argv[1];
    if (strcmp(mode, "create") == 0)
    {
        srand(time(NULL));
        int min = atoi(argv[2]);
        int max = atoi(argv[3]);
        int number = atoi(argv[4]);
        system("mkdir files");
        for (int i = 0; i < number; i++)
        {
            int a_rows = rand() % (max - min + 1) + min;
            int a_cols = rand() % (max - min + 1) + min;
            int b_cols = rand() % (max - min + 1) + min;
            char *a_name = calloc(100, sizeof(char));
            char *b_name = calloc(100, sizeof(char));
            char *c_name = calloc(100, sizeof(char));
            sprintf(a_name, "files/a%d.txt", i);
            sprintf(b_name, "files/b%d.txt", i);
            sprintf(c_name, "files/c%d.txt", i);

            generate_matrix(a_rows, a_cols, a_name);
            generate_matrix(a_cols, b_cols, b_name);

            char *command = calloc(1000, sizeof(char));
            sprintf(command, "echo \"%s %s %s\" >> lista", a_name, b_name, c_name);
            system(command);
        }
    }
    else if (strcmp(mode, "check") == 0)
    {

        char **a_filenames = calloc(100, sizeof(char *));
        char **b_filenames = calloc(100, sizeof(char *));
        char **c_filenames = calloc(100, sizeof(char *));
        FILE *input_file = fopen(argv[2], "r");
        char input_line[PATH_MAX * 3 + 3];
        int pair_counter = 0;
        while (fgets(input_line, PATH_MAX * 3 + 3, input_file) != NULL)
        {
            a_filenames[pair_counter] = calloc(PATH_MAX, sizeof(char));
            b_filenames[pair_counter] = calloc(PATH_MAX, sizeof(char));
            c_filenames[pair_counter] = calloc(PATH_MAX, sizeof(char));
            strcpy(a_filenames[pair_counter], strtok(input_line, " "));
            strcpy(b_filenames[pair_counter], strtok(NULL, " "));
            strcpy(c_filenames[pair_counter], strtok(NULL, " "));

            bool correct = check_multiply_correctness(a_filenames[pair_counter], b_filenames[pair_counter], c_filenames[pair_counter]);
            if (correct)
            {
                puts("mnozenie poprawne");
            }
            else
            {
                puts("mnozenie bledne");
            }
            pair_counter++;
        }
    }
    else
    {
        fprintf(stderr, "Error zly arg");
    }
    return 0;
}