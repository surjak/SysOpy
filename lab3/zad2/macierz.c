#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/times.h>
#include <wait.h>

const int offset = 2 * sizeof(unsigned int);

typedef struct
{
    FILE *file;
    unsigned int num_rows;
    unsigned int num_cols;
} Matrix;

Matrix *create_matrix(char *path, unsigned int rows, unsigned int cols)
{
    FILE *file = fopen(path, "w+");
    if (!file)
        return NULL;
    Matrix *matrix = malloc(sizeof(Matrix));
    matrix->file = file;
    matrix->num_rows = rows;
    matrix->num_cols = cols;
    return matrix;
}

void set_value(Matrix *matrix, double value, unsigned int r, unsigned int c)
{
    fseek(matrix->file, (matrix->num_cols * r + c) * sizeof(double) + offset, SEEK_SET);
    fwrite(&value, sizeof(double), 1, matrix->file);
}

double get_value(Matrix *matrix, unsigned int r, unsigned int c)
{
    fseek(matrix->file, (matrix->num_cols * r + c) * sizeof(double) + offset, SEEK_SET);
    double value;
    fread(&value, sizeof(double), 1, matrix->file);
    return value;
}

void print_matrix(Matrix *matrix)
{
    double v;
    for (unsigned int r = 0; r < matrix->num_rows; r++)
    {
        for (unsigned int c = 0; c < matrix->num_cols; c++)
        {
            v = get_value(matrix, r, c);
            printf("%lf ", v);
        }
        printf("\n");
    }
}

void clean_martix(Matrix *matrix)
{
    fclose(matrix->file);
    free(matrix);
}

int main()
{

    int rows = 2, cols = 3;
    Matrix *m = create_matrix("A", rows, cols);
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            set_value(m, col + row + 1, row, col);
        }
    }

    print_matrix(m);
}