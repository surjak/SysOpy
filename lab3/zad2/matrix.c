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
#include "matrix_helper.c"
#include <time.h>
#include <unistd.h>

int pair_number = 0;

typedef struct
{
    int pair_index;
    int column_index;
} Task;

Task get_task()
{
    Task task;
    task.column_index = -1;
    task.pair_index = -1;
    for (int i = 0; i < pair_number; i++)
    {
        char *task_filename = calloc(100, sizeof(char));
        sprintf(task_filename, ".tmp/tasks%d", i);
        FILE *tasks_file = fopen(task_filename, "r+");
        int fd = fileno(tasks_file);
        flock(fd, LOCK_EX);
        char *tasks = calloc(1000, sizeof(char));
        fseek(tasks_file, 0, SEEK_SET);
        fread(tasks, 1, 1000, tasks_file);
        char *task_first_zero = strchr(tasks, '0');
        int task_index = task_first_zero != NULL ? task_first_zero - tasks : -1;
        if (task_index >= 0)
        {
            char *end_of_line = strchr(tasks, '\0');
            int size = end_of_line - tasks;

            char *tasks_with_good_size = calloc(size + 1, sizeof(char));
            for (int j = 0; j < size; j++)
            {
                tasks_with_good_size[j] = tasks[j];
            }
            tasks_with_good_size[task_index] = '1';
            fseek(tasks_file, 0, SEEK_SET);
            fwrite(tasks_with_good_size, 1, size, tasks_file);
            task.pair_index = i;
            task.column_index = task_index;
            flock(fd, LOCK_UN);
            fclose(tasks_file);
            break;
        }

        flock(fd, LOCK_UN);
        fclose(tasks_file);
    }
    return task;
}

void multiply_column(char *a_file, char *b_file, int col_index, int pair_index)
{
    matrix A = load_matrix(a_file);
    matrix B = load_matrix(b_file);
    char *filename = calloc(20, sizeof(char));
    sprintf(filename, ".tmp/part%d%04d", pair_index, col_index);
    FILE *part_file = fopen(filename, "w+");
    for (int y = 0; y < A.rows; y++)
    {
        int result = 0;

        for (int x = 0; x < A.cols; x++)
        {
            result += A.values[y][x] * B.values[x][col_index];
        }

        if (y == A.rows - 1)
            fprintf(part_file, "%d", result);
        else
            fprintf(part_file, "%d\n", result);
    }
    fclose(part_file);
}

void multiply_column_to_one_file(char *a_file, char *b_file, int col_idx, char *result_file)
{
    matrix A = load_matrix(a_file);
    matrix B = load_matrix(b_file);
    FILE *file = fopen(result_file, "r+");
    int fd = fileno(file);
    flock(fd, LOCK_EX);
    matrix C = load_matrix(result_file);
    for (int y = 0; y < A.rows; y++)
    {
        int result = 0;
        for (int x = 0; x < A.cols; x++)
        {
            result += A.values[y][x] * B.values[x][col_idx];
        }
        C.values[y][col_idx] = result;
    }
    write_matrix_to_file(file, C);
    flock(fd, LOCK_UN);
    fclose(file);
}

int worker_function(char **a, char **b, int timeout, int mode, char **result_file)
{
    time_t start_time = time(NULL);
    int multiplies_count = 0;
    while (1)
    {
        if ((time(NULL) - start_time) >= timeout)
        {
            puts("timeout");
            break;
        }
        Task task = get_task();
        if (task.column_index == -1)
        {
            break;
        }
        if (mode == 1)
            multiply_column_to_one_file(a[task.pair_index], b[task.pair_index], task.column_index, result_file[task.pair_index]);
        else
            multiply_column(a[task.pair_index], b[task.pair_index], task.column_index, task.pair_index);

        multiplies_count++;
    }
    return multiplies_count;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "./matrix [list] [process number] [timeout] [mode: 1 if flock else 2]");
        return 1;
    }
    int processes_number = atoi(argv[2]);
    int timeout = atoi(argv[3]);
    int mode = atoi(argv[4]);

    char **a_filenames = calloc(100, sizeof(char *));
    char **b_filenames = calloc(100, sizeof(char *));
    char **c_filenames = calloc(100, sizeof(char *));
    system("rm -rf .tmp");
    system("mkdir -p .tmp");
    FILE *input_file = fopen(argv[1], "r");
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

        matrix a = load_matrix(a_filenames[pair_counter]);
        matrix b = load_matrix(b_filenames[pair_counter]);
        if (mode == 1)
            init_free_matrix(a.rows, b.cols, c_filenames[pair_counter]);

        char *task_filename = calloc(100, sizeof(char));
        sprintf(task_filename, ".tmp/tasks%d", pair_counter);
        FILE *tasks_file = fopen(task_filename, "w+");
        char *tasks = calloc(b.cols + 1, sizeof(char));
        sprintf(tasks, "%0*d", b.cols, 0);
        fwrite(tasks, 1, b.cols, tasks_file);
        free(tasks);
        free(task_filename);
        fclose(tasks_file);

        pair_counter++;
    }
    pair_number = pair_counter;

    pid_t *processes = calloc(processes_number, sizeof(int));
    for (int i = 0; i < processes_number; i++)
    {
        pid_t worker = fork();
        if (worker == 0)
        {
            return worker_function(a_filenames, b_filenames, timeout, mode, c_filenames);
        }
        else
        {
            processes[i] = worker;
        }
    }

    for (int i = 0; i < processes_number; i++)
    {
        int status;
        waitpid(processes[i], &status, 0);
        printf("Proces %d wykonal %d mnozen macierzy\n", processes[i],
               WEXITSTATUS(status));
    }
    free(processes);

    if (mode == 2)
    {
        for (int i = 0; i < pair_counter; i++)
        {
            char *buffer = calloc(1000, sizeof(char));
            sprintf(buffer, "paste .tmp/part%d* > %s", i, c_filenames[i]);
            system(buffer);
        }
    }

    return 0;
}