#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

typedef struct matrix
{
    char *file;
    int cols;
    int rows;
    int ocw;
} matrix;

matrix **matrices_A;
matrix **matrices_B;
matrix **matrices_C;

const int MAX_ABS_VAL = 100;
const int MAX_LINE_LEN = 100000;
const int MAX_INPUT_LINES = 100000;

double time_limit;
bool split_files;

int min(int a, int b)
{
    return a <= b ? a : b;
}

double time_diff(clock_t t1, clock_t t2)
{
    return ((double)(t2 - t1) / CLOCKS_PER_SEC);
}

int *get_column(matrix *m, FILE *f, int index)
{
    rewind(f);
    int *column = (int *)calloc(m->rows, sizeof(int));
    int i = 0;
    char *row = (char *)calloc(MAX_LINE_LEN, sizeof(char));
    char *ptr = row;
    while (fgets(row, MAX_LINE_LEN, f) != NULL)
    {
        char *elem;
        for (int j = 0; j <= index; j++)
        {
            elem = strsep(&row, " \n");
        }
        column[i++] = atoi(elem);
    }
    free(ptr);
    return column;
}

int *get_row(matrix *m, FILE *f, int index)
{
    rewind(f);
    int *row = (int *)calloc(m->cols, sizeof(int));
    char *buff = (char *)calloc(MAX_LINE_LEN, sizeof(char));
    for (int i = 0; i < index; i++)
        fgets(buff, MAX_LINE_LEN, f);
    buff = fgets(buff, MAX_LINE_LEN, f);

    char *elem;

    for (int j = 0; j < m->cols; j++)
    {
        elem = strsep(&buff, " \n");
        row[j] = atoi(elem);
    }

    return row;
}

void write_column(matrix *m, FILE *f, int *column, int index)
{
    flock(fileno(f), LOCK_EX);
    for (int i = 0; i < m->rows; i++)
    {
        int pos = m->ocw * (index + i * m->cols) + i;
        fseek(f, pos, 0);
        char *str = (char *)calloc(m->ocw, sizeof(char));
        sprintf(str, "%d", column[i]);
        int j = m->ocw - 1;
        while (str[j] == 0)
        {
            str[j] = ' ';
            j--;
        }
        fwrite(str, 1, m->ocw, f);
        free(str);
    }
    flock(fileno(f), LOCK_UN);
}

void write_column_separate(matrix *m, FILE *f, int *column, int index)
{
    for (int i = 0; i < m->rows; i++)
    {
        char *str = (char *)calloc(m->ocw, sizeof(char));
        sprintf(str, "%d", column[i]);
        int j = m->ocw - 1;
        while (str[j] == 0)
        {
            str[j] = ' ';
            j--;
        }
        fwrite(str, 1, m->ocw, f);
        fwrite("\n", 1, 1, f);
        free(str);
    }
}

int matrix_cols(FILE *f)
{
    rewind(f);
    char buffer[MAX_LINE_LEN];
    fgets(buffer, MAX_LINE_LEN, f);
    int i = 1;
    strtok(buffer, " \n");
    while (strtok(NULL, " \n") != NULL)
        i++;
    return i;
}

int matrix_rows(FILE *f)
{
    rewind(f);
    char buffer[MAX_LINE_LEN];
    int i = 0;
    while (fgets(buffer, MAX_LINE_LEN, f) != NULL)
        i++;
    return i;
}

void multiply_matrix(int col_start, int col_end, int pair_in)
{
    matrix *A = matrices_A[pair_in];
    matrix *B = matrices_B[pair_in];
    matrix *C = matrices_C[pair_in];
    FILE *f_A = fopen(A->file, "r+");
    FILE *f_B = fopen(B->file, "r+");
    FILE *f_C;
    if (!split_files)
        f_C = fopen(C->file, "r+");

    int **m_A = (int **)calloc(A->rows, sizeof(int *));
    for (int i = 0; i < A->rows; i++)
    {
        m_A[i] = get_row(A, f_A, i);
    }

    for (int col_in = col_start; col_in <= col_end; col_in++)
    {
        int *m_B_col = get_column(B, f_B, col_in);
        int *m_C_col = (int *)calloc(C->rows, sizeof(int));

        for (int i = 0; i < C->rows; i++)
        {
            int val = 0;
            for (int j = 0; j < A->cols; j++)
            {
                val += m_A[i][j] * m_B_col[j];
            }
            m_C_col[i] = val;
        }

        if (split_files)
        {
            char *file_name = (char *)calloc(100, sizeof(char));
            strcpy(file_name, C->file);
            strcat(file_name, "_");
            sprintf(file_name + strlen(file_name), "%d", col_in);
            f_C = fopen(file_name, "w+");
            free(file_name);
            write_column_separate(C, f_C, m_C_col, col_in);
        }
        else
        {
            write_column(C, f_C, m_C_col, col_in);
        }

        free(m_B_col);
        free(m_C_col);
    }
    fclose(f_A);
    fclose(f_B);
    fclose(f_C);
}

int multiply_matrices(int proc_in, int proc_count, int matrices_pair_count, clock_t start_t)
{
    int n = 0;
    for (int i = 0; i < matrices_pair_count; i++)
    {
        int col_count = matrices_B[i]->cols;
        if (proc_in < col_count)
        {
            n++;
            int col_start = proc_in * (int)ceil((double)col_count / proc_count);
            int col_end = min((proc_in + 1) * (int)ceil((double)col_count / proc_count) - 1, col_count - 1);
            multiply_matrix(col_start, col_end, i);
        }
        if (time_diff(start_t, clock()) >= time_limit)
        {
            return n;
        }
    }
    return n;
}

int main(int argc, char **argv)
{
    char *tests_list = argv[1];
    int proc_count = atoi(argv[2]);
    time_limit = strtod(argv[3], NULL);
    split_files = (atoi(argv[4]) == 2);

    pid_t *children_pids = calloc(proc_count, sizeof(pid_t));

    FILE *f = fopen(tests_list, "r");

    matrices_A = calloc(MAX_INPUT_LINES, sizeof(matrix *));
    matrices_B = calloc(MAX_INPUT_LINES, sizeof(matrix *));
    matrices_C = calloc(MAX_INPUT_LINES, sizeof(matrix *));

    int i = 0;
    FILE *tmp;
    char buffer[100];
    while (fscanf(f, "%s", buffer) != EOF)
    {
        int in = i / 3;
        switch (i % 3)
        {
        case 0:
            matrices_A[in] = malloc(sizeof(matrix));
            tmp = fopen(buffer, "r");
            matrices_A[in]->file = (char *)calloc(100, sizeof(char));
            strcpy(matrices_A[in]->file, buffer);
            matrices_A[in]->cols = matrix_cols(tmp);
            matrices_A[in]->rows = matrix_rows(tmp);
            fclose(tmp);
            break;
        case 1:
            matrices_B[in] = malloc(sizeof(matrix));
            tmp = fopen(buffer, "r");
            matrices_B[in]->file = (char *)calloc(100, sizeof(char));
            strcpy(matrices_B[in]->file, buffer);
            matrices_B[in]->cols = matrix_cols(tmp);
            matrices_B[in]->rows = matrix_rows(tmp);
            fclose(tmp);
            break;
        case 2:
            matrices_C[in] = malloc(sizeof(matrix));
            tmp = fopen(buffer, "w+");
            matrices_C[in]->file = (char *)calloc(100, sizeof(char));
            strcpy(matrices_C[in]->file, buffer);
            matrices_C[in]->cols = matrices_B[in]->cols;
            matrices_C[in]->rows = matrices_A[in]->rows;

            int cols = matrices_C[in]->cols;
            int rows = matrices_C[in]->rows;
            int ocw = (int)ceil(log10(matrices_A[in]->cols * MAX_ABS_VAL * MAX_ABS_VAL)) + 3;

            matrices_C[in]->ocw = ocw;

            char *buf = (char *)calloc(rows * (cols * ocw + 1) + 1, sizeof(char));
            strcpy(buf, "");
            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols * ocw; j++)
                {
                    strcat(buf, "@");
                }
                strcat(buf, "\n");
            }
            fwrite(buf, 1, (cols * ocw + 1) * rows, tmp);
            free(buf);
            fclose(tmp);
            break;
        }
        i++;
    }

    int matrices_pairs = i / 3;

    fclose(f);

    for (int i = 0; i < proc_count; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            int matrices_multiplied = multiply_matrices(i, proc_count, matrices_pairs, clock());
            exit(matrices_multiplied);
        }
        else
        {
            children_pids[i] = child_pid;
        }
    }

    for (int i = 0; i < proc_count; i++)
    {
        int status;
        waitpid(children_pids[i], &status, 0);
        printf("Process %d made %d operations\n", children_pids[i], WEXITSTATUS(status));
    }

    if (split_files)
    {
        for (int i = 0; i < matrices_pairs; i++)
        {
            pid_t child_pid = fork();

            if (child_pid == 0)
            {
                char **args = (char **)calloc(matrices_C[i]->cols + 2, sizeof(char *));
                args[0] = (char *)calloc(6, sizeof(char));
                strcpy(args[0], "paste");
                for (int j = 0; j < matrices_C[i]->cols; j++)
                {
                    args[j + 1] = (char *)calloc(100, sizeof(char));
                    strcpy(args[j + 1], matrices_C[i]->file);
                    strcat(args[j + 1], "_");
                    sprintf(args[j + 1] + strlen(args[j + 1]), "%d", j);
                }
                args[matrices_C[i]->cols + 1] = NULL;

                int fd = open(matrices_C[i]->file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                close(fd);

                execvp(args[0], args);
            }
            else
            {
                waitpid(child_pid, NULL, 0);
            }
        }
    }

    return 0;
}