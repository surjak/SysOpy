#include "library.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>

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
    fprintf(f, "%30s\t\t%15s\t%15s\t%15s\t%15s\t%15s\n",
            "Name",
            "Real [s]",
            "User [s]",
            "System [s]",
            "Child User [s]",
            "Child System [s]");
}

void save_timer(char *name, FILE *f)
{
    end_timer();
    int clk_tics = sysconf(_SC_CLK_TCK);
    double real_time = (double)(en_time - st_time) / clk_tics;
    double user_time = (double)(en_cpu.tms_utime - st_cpu.tms_utime) / clk_tics;
    double system_time = (double)(en_cpu.tms_stime - st_cpu.tms_stime) / clk_tics;
    double child_user_time = (double)(en_cpu.tms_cutime - st_cpu.tms_cutime) / clk_tics;
    double child_system_time = (double)(en_cpu.tms_cstime - st_cpu.tms_cstime) / clk_tics;
    fprintf(f, "%30s:\t\t%15f\t%15f\t%15f\t%15f\t%15f\t\n",
            name,
            real_time,
            user_time,
            system_time,
            child_user_time,
            child_system_time);
}

int main(int argc, char *argv[])
{

    char file_name[] = "raport.txt";
    report_file = fopen(file_name, "a");

    write_file_header(report_file);

    // int i = 1;
    // while (i < argc)
    // {
    //     if (!strcmp(argv[i], "create_table"))
    //     {
    //         int err = parse_create_table(argv, i, argc);
    //         if (err < 0)
    //         {
    //             fprintf(stderr, "function create_table returned error, stopping\n");
    //             return -1;
    //         }
    //         i += 2;
    //     }
    //     else if (!strcmp(argv[i], "search_directory"))
    //     {
    //         int err = parse_search_directory(argv, i, argc);
    //         if (err < 0)
    //         {
    //             fprintf(stderr, "function search_directory returned error, stopping\n");
    //             return -1;
    //         }
    //         i += 4;
    //     }
    //     else if (!strcmp(argv[i], "load_to_array"))
    //     {
    //         int err = parse_load_to_array(argv, i, argc);
    //         if (err < 0)
    //         {
    //             fprintf(stderr, "function load_to_array returned error, stopping\n");
    //             return -1;
    //         }
    //         i += 1;
    //     }
    //     else if (!strcmp(argv[i], "remove_block"))
    //     {
    //         int err = parse_remove_block(argv, i, argc);
    //         if (err < 0)
    //         {
    //             fprintf(stderr, "function remove_block returned error, stopping\n");
    //             return -1;
    //         }
    //         i += 2;
    //     }
    //     else if (!strcmp(argv[i], "start"))
    //     {
    //         start_timer();
    //         i += 1;
    //     }
    //     else if (!strcmp(argv[i], "end"))
    //     {
    //         parse_end_timer(argv, i, argc);
    //         i += 2;
    //     }
    //     else
    //     {
    //         fprintf(stderr, "Wrong command\n");
    //         return -1;
    //     }
    // }
    // delete_array();

    return 0;
}