#include "generator.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/times.h>
#include <string.h>
#include <stdlib.h>

clock_t st_time, en_time;
struct tms st_cpu, en_cpu;
FILE *report_file;

void start_timer()
{
    st_time = times(&st_cpu);
}

void end_timer()
{
    en_time = times(&en_cpu);
}

void write_file_header(FILE *f)
{
    fprintf(f, "%40s\t\t%15s\t%15s\t%15s\n",
            "Name",
            "Real [s]",
            "User [s]",
            "System [s]");
}

void save_timer(char *name, FILE *f)
{
    int clk_tics = sysconf(_SC_CLK_TCK);
    double real_time = (double)(en_time - st_time) / clk_tics;
    double user_time = (double)(en_cpu.tms_utime - st_cpu.tms_utime) / clk_tics;
    double system_time = (double)(en_cpu.tms_stime - st_cpu.tms_stime) / clk_tics;

    fprintf(f, "%40s:\t\t%15f\t%15f\t%15f\n",
            name,
            real_time,
            user_time,
            system_time);
}

int parse_generate(int argc, char *argv[], int i)
{
    if (i + 3 > argc)
    {
        fprintf(stderr, "generate command wrong args\n");
        exit(-1);
    }
    char *file_name = argv[i + 1];
    int rec_num = atoi(argv[i + 2]);
    int byte_num = atoi(argv[i + 3]);
    generate(file_name, rec_num, byte_num);
    return i + 4;
}

int parse_sort(int argc, char *argv[], int i)
{
    if (i + 4 >= argc)
    {
        fprintf(stderr, "sort command wrong args\n");
        exit(-1);
    }
    char *file_name = argv[i + 1];
    int rec_num = atoi(argv[i + 2]);
    int byte_num = atoi(argv[i + 3]);
    char *lib_sys = argv[i + 4];

    if (!strcmp(lib_sys, "lib"))
    {
        start_timer();
        sort_lib(file_name, rec_num, byte_num);
        end_timer();
    }
    else if (!strcmp(lib_sys, "sys"))
    {
        start_timer();
        sort_sys(file_name, rec_num, byte_num);
        end_timer();
    }
    char buff[64];
    snprintf(buff, sizeof buff, "sort %s, records: %d, bytes: %d", lib_sys, rec_num, byte_num);
    save_timer(buff, report_file);

    return i + 5;
}

int parse_copy(int argc, char *argv[], int i)
{
    if (i + 5 >= argc)
    {
        fprintf(stderr, "sort command wrong args\n");
        exit(-1);
    }

    char *file_from = argv[i + 1];
    char *file_to = argv[i + 2];
    int rec_num = atoi(argv[i + 3]);
    int byte_num = atoi(argv[i + 4]);
    char *lib_sys = argv[i + 5];

    if (!strcmp(lib_sys, "lib"))
    {
        start_timer();
        copy_lib(file_from, file_to, rec_num, byte_num);
        end_timer();
    }
    else if (!strcmp(lib_sys, "sys"))
    {
        start_timer();
        copy_sys(file_from, file_to, rec_num, byte_num);
        end_timer();
    }

    char buff[64];
    snprintf(buff, sizeof buff, "copy %s, records: %d, bytes: %d", lib_sys, rec_num, byte_num);
    save_timer(buff, report_file);

    return i + 6;
}

int main(int argc, char *argv[])
{
    char file_name[] = "wyniki.txt";

    if (access(file_name, F_OK) != 0)
    {
        report_file = fopen(file_name, "a");
        write_file_header(report_file);
    }
    else
    {
        report_file = fopen(file_name, "a");
    }

    int i = 1;
    while (i < argc)
    {
        if (!strcmp(argv[i], "generate"))
        {
            i = parse_generate(argc, argv, i);
        }
        else if (!strcmp(argv[i], "sort"))
        {
            i = parse_sort(argc, argv, i);
        }
        else if (!strcmp(argv[i], "copy"))
        {
            i = parse_copy(argc, argv, i);
        }
        else
        {
            fprintf(stderr, "Wrong command\n");
            exit(-1);
        }
    }
    fclose(report_file);
    return 0;
}