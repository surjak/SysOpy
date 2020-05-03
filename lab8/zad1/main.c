#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <time.h>
int W;
int H;
int **image;

int threads;
int **histogram;
struct arg_struct
{
    int arg1;
    char *mode;
};

void set_W_and_H(char *buffer)
{
    char *w;
    char *h;
    char *rest = buffer;

    w = strtok_r(rest, " \t\r\n", &rest);
    h = strtok_r(rest, " \t\r\n", &rest);
    W = atoi(w);
    H = atoi(h);
}
void load_row_to_array(char *line, int r)
{
    char *i;
    char *rest = line;
    for (int col = 0; col < W; col++)
    {
        i = strtok_r(rest, " \t\r\n", &rest);
        image[r][col] = atoi(i);
    }
}
void load_image_to_array(char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        fprintf(stderr, "Cant open file \n");
        exit(EXIT_FAILURE);
    }
    int MAX_LINE_LENGTH = 128;
    char buffer[MAX_LINE_LENGTH + 1];
    // skip header
    fgets(buffer, MAX_LINE_LENGTH, f);
    // skip comments
    do
    {
        fgets(buffer, MAX_LINE_LENGTH, f);
    } while (buffer[0] == '#');
    set_W_and_H(buffer);
    // skip M
    fgets(buffer, MAX_LINE_LENGTH, f);

    image = calloc(H, sizeof(int *));
    for (int i = 0; i < H; i++)
        image[i] = calloc(W, sizeof(int));

    char *line = NULL;
    size_t len = 0;
    for (int r = 0; r < H; r++)
    {
        getline(&line, &len, f);
        load_row_to_array(line, r);
    }
    fclose(f);
}

void *sign(int index)
{
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int size = 255 / threads;
    for (int w = 0; w < H; w++)
    {
        for (int c = 0; c < W; c++)
        {
            if (image[w][c] / size == index)
            {
                histogram[index][image[w][c]]++;
            }
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    return (void *)time;
}

void *block(int index)
{
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int size = W / threads;
    for (int col = index * size; col < (index + 1) * size; col++)
    {
        for (int w = 0; w < H; w++)
        {
            histogram[index][image[w][col]]++;
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    return (void *)time;
}
void *interleaved(int index)
{
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    for (int col = index; col < W; col += threads)
    {
        for (int w = 0; w < H; w++)
        {
            histogram[index][image[w][col]]++;
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double *time = malloc(sizeof(double));
    *time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    return (void *)time;
}
void *write_to_histogram(void *arg)
{
    struct arg_struct *args = arg;
    if (strcmp(args->mode, "sign") == 0)
    {
        return sign(args->arg1);
    }
    else if (strcmp(args->mode, "block") == 0)
    {
        return block(args->arg1);
    }
    else if (strcmp(args->mode, "interleaved") == 0)
    {
        return interleaved(args->arg1);
    }
    else
    {
        printf("unknow mode\n");
        exit(EXIT_FAILURE);
    }
}
void save_histogram(char *output_file)
{
    FILE *f = fopen(output_file, "w");

    for (int i = 0; i < 256; i++)
    {
        int sum = 0;
        for (int j = 0; j < threads; j++)
        {
            sum += histogram[j][i];
        }
        fprintf(f, "%d -> %d\n", i, sum);
    }
    fclose(f);
}
int main(int argc, char *argv[])
{

    threads = atoi(argv[1]);
    char *mode = argv[2];
    char *input_file = argv[3];
    char *output_file = argv[4];
    char buffer[256];
    load_image_to_array(input_file);

    FILE *txt = fopen("Times.txt", "a");
    fprintf(txt, "Mode: %s | threads: %d\n", mode, threads);

    histogram = calloc(threads, sizeof(int *));
    for (int i = 0; i < threads; i++)
        histogram[i] = calloc(256, sizeof(int));

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    pthread_t *thread_ids = calloc(threads, sizeof(pthread_t));
    struct arg_struct *args = calloc(threads, sizeof(struct arg_struct));
    for (int i = 0; i < threads; i++)
    {
        struct arg_struct arg;
        arg.arg1 = i;
        arg.mode = mode;

        args[i] = arg;

        pthread_create(&thread_ids[i], NULL, write_to_histogram, (void *)&args[i]);
    }

    for (int i = 0; i < threads; i++)
    {
        double *y;
        pthread_join(thread_ids[i], (void *)&y);
        printf("Thread %d ------- %lf microseconds\n", i, *y);
        fprintf(txt, "Thread %d ------- %lf microseconds\n", i, *y);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    double time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000.0;
    printf("\nFULL TIME: %f\n", time);
    fprintf(txt, "FULL TIME: %f\n\n", time);

    save_histogram(output_file);

    // clean
    for (int i = 0; i < H; i++)
        free(image[i]);
    free(image);
    for (int i = 0; i < threads; i++)
        free(histogram[i]);
    free(histogram);
    fclose(txt);
    return 0;
}