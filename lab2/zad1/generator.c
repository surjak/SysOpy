#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

void generate(const char *file_name, const unsigned int num_records, const unsigned int byte_num)
{
    char buff[64];
    // snprintf(buff, sizeof buff, "head -c %d /dev/urandom > %s", num_records * byte_num, file_name);

    snprintf(buff, sizeof buff, "</dev/urandom tr -dc 'A-Z0-9a-z' | head -c %d > %s", num_records * byte_num, file_name);
    int find_status = system(buff);
    if (find_status != 0)
    {
        fprintf(stderr, "error while generating: %s\n", strerror(errno));
        exit(-1);
    }
}

void swap_in_file(FILE *f, const unsigned int num_records, const unsigned int byte_num, unsigned int i, unsigned int j)
{

    char *tmp1 = malloc(byte_num);
    char *tmp2 = malloc(byte_num);
    if (fseek(f, i * byte_num, SEEK_SET) < 0)
    {
        fprintf(stderr, "1cant seek sort lib: %s\n", strerror(errno));
        exit(-1);
    }
    if (fread(tmp1, 1, byte_num, f) != byte_num)
    {
        fprintf(stderr, "2cant read sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (fseek(f, (j)*byte_num, SEEK_SET) < 0)
    {
        fprintf(stderr, "3cant seek sort lib: %s\n", strerror(errno));
        exit(-1);
    }
    if (fread(tmp2, 1, byte_num, f) != byte_num)
    {
        printf("%d : %d", i, j);

        fprintf(stderr, "4cant read sort lib: %s\n", strerror(errno));
        exit(-1);
    }
    // printf("%d : %d\n", i, j);
    // printf("%s : %s\n", tmp1, tmp2);

    if (fseek(f, i * byte_num, SEEK_SET) < 0)
    {
        fprintf(stderr, "5cant fseek sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (fwrite(tmp2, 1, byte_num, f) != byte_num)
    {
        fprintf(stderr, "error while writing to file sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (fseek(f, (j)*byte_num, SEEK_SET) < 0)
    {
        fprintf(stderr, "error while fseek sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (fwrite(tmp1, 1, byte_num, f) != byte_num)
    {
        fprintf(stderr, "error while fwrite sort lib: %s\n", strerror(errno));
        exit(-1);
    }
    free(tmp1);
    free(tmp2);
}
int partition(FILE *f, const unsigned int num_records, const unsigned int byte_num, unsigned int low, unsigned int high)
{

    char *tmp1 = malloc(byte_num);
    char *tmp2 = malloc(byte_num);
    if (fseek(f, (high)*byte_num, SEEK_SET) < 0)
    {
        fprintf(stderr, "cant seek sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (fread(tmp1, 1, byte_num, f) != byte_num)
    {
        // printf("%d : %d", low, high);
        fprintf(stderr, "6cant read sort lib: %s\n", strerror(errno));
        exit(-1);
    }
    // printf("%d : %s", low, tmp1);

    unsigned char min_char = tmp1[0];
    int i = low - 1;

    for (int j = low; j < high; j++)
    {
        if (fseek(f, j * byte_num, SEEK_SET) < 0)
        {
            fprintf(stderr, "cant seek sort lib: %s\n", strerror(errno));
            exit(-1);
        }
        if (fread(tmp2, 1, byte_num, f) != byte_num)
        {
            fprintf(stderr, "7cant read sort lib: %s\n", strerror(errno));
            exit(-1);
        }

        if (tmp2[0] < min_char)
        {
            i++;
            swap_in_file(f, num_records, byte_num, i, j);
        }
    }
    swap_in_file(f, num_records, byte_num, i + 1, high);
    free(tmp1);
    free(tmp2);
    return (i + 1);
}
void qSort(FILE *f, const unsigned int num_records, const unsigned int byte_num, unsigned int low, unsigned int high)
{

    if (low < high)
    {
        int pi = partition(f, num_records, byte_num, low, high);
        if (pi != 0)
            qSort(f, num_records, byte_num, low, pi - 1);
        if (pi != num_records)
            qSort(f, num_records, byte_num, pi + 1, high);
    }
}

void pack(const char *file_name, const unsigned int num_records, const unsigned int byte_num)
{

    FILE *f = fopen(file_name, "r+");

    if (f == NULL)
    {
        fprintf(stderr, "Can't open file sort lib: %s \n", strerror(errno));
        exit(-1);
    }
    qSort(f, num_records, byte_num, 0, num_records - 1);

    if (fseek(f, 0, SEEK_SET) < 0)
    {
        fprintf(stderr, "cant seek sort lib: %s\n", strerror(errno));
        exit(-1);
    }
    char *tmp1 = malloc(num_records * byte_num);
    if (fread(tmp1, 1, num_records * byte_num, f) != num_records * byte_num)
    {
        fprintf(stderr, "8cant read sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    printf("%s", tmp1);
    for (int i = 0; i < num_records * byte_num - byte_num; i += byte_num)
    {
        if (tmp1[i] > tmp1[i + byte_num])
            printf("ZLE: %s", tmp1);
    }

    fclose(f);
}

int main()
{
    generate("tmp.txt", 50, 5);

    pack("tmp.txt", 50, 5);
    return 0;
}