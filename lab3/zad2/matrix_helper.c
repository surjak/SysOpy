#define _XOPEN_SOURCE 500
#define MAX_COLS_NUMBER 1000
#define MAX_LINE_LENGTH (MAX_COLS_NUMBER * 5)
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct
{
    int **values;
    int rows;
    int cols;
} matrix;

int get_cols_number(char *row)
{
    int cols = 0;
    char *num = strtok(row, " ");
    while (num != NULL)
    {
        if (strcmp(num, "\n") != 0)
            cols++;
        num = strtok(NULL, " ");
    }
    return cols;
}

void set_cols_and_rows(FILE *f, int *rows, int *cols)
{
    char line[MAX_LINE_LENGTH];
    *rows = 0;
    *cols = 0;
    while (fgets(line, MAX_LINE_LENGTH, f) != NULL)
    {
        if (*cols == 0)
        {
            *cols = get_cols_number(line);
        }
        *rows = *rows + 1;
    }
    fseek(f, 0, SEEK_SET);
}

matrix load_matrix(char *filename)
{
    FILE *file = fopen(filename, "r");
    int rows, cols;
    set_cols_and_rows(file, &rows, &cols);
    int **values = calloc(rows, sizeof(int *));
    for (int i = 0; i < rows; i++)
    {
        values[i] = calloc(cols, sizeof(int));
    }
    int x, y = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {
        x = 0;
        char *number = strtok(line, " \t\n");

        while (number != NULL)
        {
            values[y][x++] = atoi(number);
            number = strtok(NULL, " \t\n");
        }
        y++;
    }

    fclose(file);
    matrix m;
    m.values = values;
    m.rows = rows;
    m.cols = cols;
    return m;
}

void free_matrix(matrix *m)
{
    for (int y = 0; y < m->rows; y++)
    {
        free(m->values[y]);
    }
    free(m->values);
}

void print_matrix(matrix m)
{
    for (int i = 0; i < m.rows; i++)
    {
        for (int j = 0; j < m.cols; j++)
        {
            printf("%d ", m.values[i][j]);
        }
        printf("\n");
    }
}

matrix multiply_matrices(matrix A, matrix B)
{
    int **values = calloc(A.rows, sizeof(int *));
    for (int i = 0; i < A.rows; i++)
    {
        values[i] = calloc(B.cols, sizeof(int));
    }
    for (int i = 0; i < A.rows; i++)
    {
        for (int j = 0; j < B.cols; j++)
        {
            int result = 0;
            for (int k = 0; k < A.cols; k++)
            {
                result += (A.values[i][k] * B.values[k][j]);
            }
            values[i][j] = result;
        }
    }

    matrix m;
    m.values = values;
    m.rows = A.rows;
    m.cols = B.cols;

    return m;
}

void generate_matrix(int rows, int cols, char *filename)
{
    FILE *file = fopen(filename, "w+");

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            if (x > 0)
            {
                fprintf(file, " ");
            }

            fprintf(file, "%d", rand() % (200 + 1) - 100);
        };
        fprintf(file, "\n");
    }
    fclose(file);
}

void init_free_matrix(int rows, int cols, char *filename)
{
    FILE *file = fopen(filename, "w+");

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            if (x > 0)
            {
                fprintf(file, " ");
            }

            fprintf(file, "%d", 0);
        };
        fprintf(file, "\n");
    }
    fclose(file);
}

void write_matrix_to_file(FILE *file, matrix a)
{
    fseek(file, 0, SEEK_SET);
    for (int y = 0; y < a.rows; y++)
    {
        for (int x = 0; x < a.cols; x++)
        {
            if (x > 0)
            {
                fprintf(file, " ");
            }
            fprintf(file, "%d", a.values[y][x]);
        };
        fprintf(file, "\n");
    }
}