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
#include <sys/file.h>
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
    fwrite(&matrix->num_rows, sizeof(unsigned int), 1, file);
    fwrite(&matrix->num_cols, sizeof(unsigned int), 1, file);

    fseek(file, 0, SEEK_SET);
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

Matrix *open_martix_from_file(char *file_path)
{
    FILE *file = fopen(file_path, "r+");
    if (!file)
        return NULL;
    Matrix *matrix = malloc(sizeof(Matrix));
    fread(&matrix->num_rows, sizeof(unsigned int), 1, file);
    fread(&matrix->num_cols, sizeof(unsigned int), 1, file);
    matrix->file = file;
    return matrix;
}

void multiply_i_column(Matrix *A, Matrix *B, int i, int count_of_processes, Matrix **C)
{

    int fd;
    fd = fileno((*C)->file);

    struct flock lock;
    lock.l_type = F_WRLCK;

    fcntl(fd, F_SETLKW, &lock);
    for (int c = 0; c < (*C)->num_cols; c++)
    {
        for (int r = 0; r < (*C)->num_rows; r++)
        {

            set_value(*C, 1.0, r, c);
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
}

// for (int j = i; j < B->num_cols; j += count_of_processes)
// {
//     for (int r_a = 0; r_a < A->num_rows; r_a++)
//     {
//         double value = 0.0;
//         for (int c = 0; c < A->num_cols; c++)
//         {
//             for (int r = 0; r < B->num_rows; r++)
//             {
//                 // printf("%d:%d:     ", r, j);
//                 // printf("%f\n", get_value(B, r, j));
//                 value += get_value(A, r_a, c) * get_value(B, r, j);
//             }
//         }
//         set_value(*C, 1.0, r_a, j);
//     }
//     printf("%d\n", j);
// }
// }
// printf("%d:%d:\n", getpid(), j);
Matrix *multiply_matrices(Matrix *A, Matrix *B, char *output, unsigned int count_of_processes, double seconds)
{
    Matrix *C = create_matrix(output, A->num_rows, B->num_cols);
    int columns_per_process = B->num_cols / count_of_processes;
    if (columns_per_process < 1)
        count_of_processes = B->num_cols;

    for (int i = 0; i < count_of_processes; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            multiply_i_column(A, B, i, count_of_processes, &C);

            exit(0);
        }
    }

    for (int i = 0; i < count_of_processes; i++)
        wait(NULL);
    return C;
}

int main()
{

    int rows = 4, cols = 4;
    Matrix *m = create_matrix("A", rows, cols);
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            double value = rand() % 3;
            set_value(m, value, row, col);
        }
    }
    Matrix *b = create_matrix("B", rows, cols);
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            double value = rand() % 3;
            set_value(b, value, row, col);
        }
    }
    // Matrix *m = open_martix_from_file("A");
    print_matrix(m);
    printf("\n\n\n");

    print_matrix(b);
    printf("\n\n\n");

    Matrix *C = multiply_matrices(m, b, "C", 3, 3.0);

    print_matrix(C);
}