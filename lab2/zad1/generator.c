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

    snprintf(buff, sizeof buff, "</dev/urandom tr -dc 'A-Z0-9a-z' | head -c %d > %s", num_records * byte_num, file_name);
    int find_status = system(buff);
    if (find_status != 0)
    {
        fprintf(stderr, "error while generating: %s\n", strerror(errno));
        exit(-1);
    }
}

void swap_in_file_lib(FILE *f, const unsigned int num_records, const unsigned int byte_num, unsigned int i, unsigned int j)
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
        fprintf(stderr, "4cant read sort lib: %s\n", strerror(errno));
        exit(-1);
    }

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
int partition_lib(FILE *f, const unsigned int num_records, const unsigned int byte_num, unsigned int low, unsigned int high)
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

        fprintf(stderr, "6cant read sort lib: %s\n", strerror(errno));
        exit(-1);
    }

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
            swap_in_file_lib(f, num_records, byte_num, i, j);
        }
    }
    swap_in_file_lib(f, num_records, byte_num, i + 1, high);
    free(tmp1);
    free(tmp2);
    return (i + 1);
}
void qSort_lib(FILE *f, const unsigned int num_records, const unsigned int byte_num, unsigned int low, unsigned int high)
{

    if (low < high)
    {
        int pi = partition_lib(f, num_records, byte_num, low, high);
        if (pi != 0)
            qSort_lib(f, num_records, byte_num, low, pi - 1);
        if (pi != num_records)
            qSort_lib(f, num_records, byte_num, pi + 1, high);
    }
}

void sort_lib(const char *file_name, const unsigned int num_records, const unsigned int byte_num)
{

    FILE *f = fopen(file_name, "r+");

    if (f == NULL)
    {
        fprintf(stderr, "Can't open file sort lib: %s \n", strerror(errno));
        exit(-1);
    }
    qSort_lib(f, num_records, byte_num, 0, num_records - 1);

    fclose(f);
}

void swap_in_file_sys(int f, const unsigned int num_records, const unsigned int byte_num, unsigned int i, unsigned int j)
{

    char *tmp1 = malloc(byte_num);
    char *tmp2 = malloc(byte_num);
    if (lseek(f, i * byte_num, SEEK_SET) < 0)
    {
        fprintf(stderr, "1cant seek sort lib: %s\n", strerror(errno));
        exit(-1);
    }
    if (read(f, tmp1, byte_num) < 0)
    {
        fprintf(stderr, "2cant read sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (lseek(f, (j)*byte_num, SEEK_SET) < 0)
    {
        fprintf(stderr, "3cant seek sort lib: %s\n", strerror(errno));
        exit(-1);
    }
    if (read(f, tmp2, byte_num) < 0)
    {

        fprintf(stderr, "4cant read sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (lseek(f, i * byte_num, SEEK_SET) < 0)
    {
        fprintf(stderr, "5cant fseek sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (write(f, tmp2, byte_num) < 0)
    {
        fprintf(stderr, "error while writing to file sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (lseek(f, (j)*byte_num, SEEK_SET) < 0)
    {
        fprintf(stderr, "error while fseek sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (write(f, tmp1, byte_num) < 0)
    {
        fprintf(stderr, "error while fwrite sort lib: %s\n", strerror(errno));
        exit(-1);
    }
    free(tmp1);
    free(tmp2);
}

int partition_sys(int f, const unsigned int num_records, const unsigned int byte_num, unsigned int low, unsigned int high)
{

    char *tmp1 = malloc(byte_num);
    char *tmp2 = malloc(byte_num);
    if (lseek(f, (high)*byte_num, SEEK_SET) < 0)
    {
        fprintf(stderr, "cant seek sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    if (read(f, tmp1, byte_num) < 0)
    {

        fprintf(stderr, "6cant read sort lib: %s\n", strerror(errno));
        exit(-1);
    }

    unsigned char min_char = tmp1[0];
    int i = low - 1;

    for (int j = low; j < high; j++)
    {
        if (lseek(f, j * byte_num, SEEK_SET) < 0)
        {
            fprintf(stderr, "cant seek sort lib: %s\n", strerror(errno));
            exit(-1);
        }
        if (read(f, tmp2, byte_num) < 0)
        {
            fprintf(stderr, "7cant read sort lib: %s\n", strerror(errno));
            exit(-1);
        }

        if (tmp2[0] < min_char)
        {
            i++;
            swap_in_file_sys(f, num_records, byte_num, i, j);
        }
    }
    swap_in_file_sys(f, num_records, byte_num, i + 1, high);
    free(tmp1);
    free(tmp2);
    return (i + 1);
}

void qSort_sys(int f, const unsigned int num_records, const unsigned int byte_num, unsigned int low, unsigned int high)
{

    if (low < high)
    {
        int pi = partition_sys(f, num_records, byte_num, low, high);
        if (pi != 0)
            qSort_sys(f, num_records, byte_num, low, pi - 1);
        if (pi != num_records)
            qSort_sys(f, num_records, byte_num, pi + 1, high);
    }
}

void sort_sys(const char *file_name, const unsigned int num_records, const unsigned int byte_num)
{
    int f = open(file_name, O_RDWR);
    if (f < 0)
    {
        fprintf(stderr, "cant open file sort sys: %s\n", strerror(errno));
        exit(-1);
    }
    qSort_sys(f, num_records, byte_num, 0, num_records - 1);

    close(f);
}

void copy_sys(const char *file_from, const char *file_to, const unsigned int num_records, const unsigned int byte_num)
{
    int from = open(file_from, O_RDONLY);
    if (from < 0)
    {
        fprintf(stderr, "cant open source file copy sys: %s\n", strerror(errno));
        exit(-1);
    }
    int to = open(file_to, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (to < 0)
    {
        fprintf(stderr, "cant open destination file copy sys: %s\n", strerror(errno));
        exit(-1);
    }

    unsigned char *holder = malloc(byte_num);
    for (int i = 0; i < num_records; ++i)
    {

        if (read(from, holder, byte_num) < 0)
        {
            fprintf(stderr, "cant read from source file copy sys: %s\n", strerror(errno));
            exit(-1);
        }
        if (write(to, holder, byte_num) < 0)
        {
            fprintf(stderr, "cant write to dst file copy sys: %s\n", strerror(errno));
            exit(-1);
        }
    }
    free(holder);
    close(from);
    close(to);
}

void copy_lib(const char *file_from, const char *file_to, const unsigned int num_records, const unsigned int byte_num)
{
    FILE *src_file = fopen(file_from, "r");
    if (src_file == NULL)
    {
        fprintf(stderr, "cant open source file copy lib: %s\n", strerror(errno));
        exit(-1);
    }
    FILE *dst_file = fopen(file_to, "w");
    if (dst_file == NULL)
    {
        fprintf(stderr, "cant open dst file copy lib: %s\n", strerror(errno));
        exit(-1);
    }

    unsigned char *holder = malloc(byte_num);
    for (int i = 0; i < num_records; i++)
    {
        if (fread(holder, 1, byte_num, src_file) != byte_num)
        {
            fprintf(stderr, "cant read from source file copy lib: %s\n", strerror(errno));
            exit(-1);
        }
        if (fwrite(holder, 1, byte_num, dst_file) != byte_num)
        {
            fprintf(stderr, "cant write to dst file copy lib: %s\n", strerror(errno));
            exit(-1);
        }
    }
    free(holder);
    fclose(src_file);
    fclose(dst_file);
}
