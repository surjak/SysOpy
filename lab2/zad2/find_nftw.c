
#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

const char format[] = "%Y-%m-%d %H:%M:%S";
char follow_mode;
char *command;
time_t follow_date;
int maxdepth;

void print_from_stat(const char *filename, const struct stat *statptr)
{
    char file_type[64] = "undefined";

    if (S_ISREG(statptr->st_mode))
        strcpy(file_type, "file");
    else if (S_ISDIR(statptr->st_mode))
        strcpy(file_type, "dir");
    else if (S_ISLNK(statptr->st_mode))
        strcpy(file_type, "slink");
    else if (S_ISCHR(statptr->st_mode))
        strcpy(file_type, "char dev");
    else if (S_ISBLK(statptr->st_mode))
        strcpy(file_type, "block dev");
    else if (S_ISFIFO(statptr->st_mode))
        strcpy(file_type, "fifo");
    else if (S_ISSOCK(statptr->st_mode))
        strcpy(file_type, "socket");

    struct tm tm_modif_time;
    localtime_r(&statptr->st_mtime, &tm_modif_time);
    char modif_time_str[255];
    strftime(modif_time_str, 255, format, &tm_modif_time);

    struct tm tm_access_time;
    localtime_r(&statptr->st_atime, &tm_access_time);
    char access_time_str[255];
    strftime(access_time_str, 255, format, &tm_access_time);

    printf("%s || type: %s, size: %ld, modification time: %s, access time: %s\n",
           filename, file_type, statptr->st_size, modif_time_str, access_time_str);
}

int file_info(const char *filename, const struct stat *statptr,
              int fileflags, struct FTW *pfwt)
{

    if (maxdepth != -1 && pfwt->level > maxdepth)
    {

        return 0;
    }
    if (strcmp(command, "maxdepth") == 0)
    {
        print_from_stat(filename, statptr);

        return 0;
    }
    else if (strcmp(command, "mtime") == 0)
    {

        time_t modif_time = statptr->st_mtime;

        if (follow_mode == '-')
        {
            int diff_modif = difftime(follow_date, modif_time);
            if (!((diff_modif == 0 && follow_mode == '=') || (diff_modif > 0 && follow_mode == '+') || (diff_modif < 0 && follow_mode == '-')))
                return 0;
            ;

            print_from_stat(filename, statptr);
        }
        else if (follow_mode == '+')
        {
            int diff_modif = difftime(follow_date, modif_time);
            if (!((diff_modif == 0 && follow_mode == '=') || (diff_modif > 0 && follow_mode == '+') || (diff_modif < 0 && follow_mode == '-')))
                return 0;

            print_from_stat(filename, statptr);
        }
        else if (follow_mode == '=')
        {

            int diff_modif = abs(difftime(modif_time, follow_date)) / (60 * 60 * 24);
            int diff_modif2 = difftime(follow_date, modif_time);

            if ((diff_modif == 0 && follow_mode == '=') && !(diff_modif2 < 0 && follow_mode == '='))
                print_from_stat(filename, statptr);
            return 0;
        }
    }
    else if (strcmp(command, "atime") == 0)
    {
        time_t modif_time = statptr->st_atime;

        if (follow_mode == '-')
        {
            int diff_modif = difftime(follow_date, modif_time);
            if (!((diff_modif == 0 && follow_mode == '=') || (diff_modif > 0 && follow_mode == '+') || (diff_modif < 0 && follow_mode == '-')))
                return 0;
            ;

            print_from_stat(filename, statptr);
        }
        else if (follow_mode == '+')
        {
            int diff_modif = difftime(follow_date, modif_time);
            if (!((diff_modif == 0 && follow_mode == '=') || (diff_modif > 0 && follow_mode == '+') || (diff_modif < 0 && follow_mode == '-')))
                return 0;

            print_from_stat(filename, statptr);
        }
        else if (follow_mode == '=')
        {

            int diff_modif = abs(difftime(modif_time, follow_date)) / (60 * 60 * 24);
            int diff_modif2 = difftime(follow_date, modif_time);

            if ((diff_modif == 0 && follow_mode == '=') && !(diff_modif2 < 0 && follow_mode == '='))
                print_from_stat(filename, statptr);
            return 0;
        }
    }
    return 0;
}

void print_files_handler(char *root_path, char *comm, char mode, int count, int maxdep)
{

    int flags = FTW_PHYS;
    int fd_limit = 2;
    follow_mode = mode;
    command = comm;
    maxdepth = maxdep;

    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    timeinfo->tm_mday -= count;
    if (mode == '+')
    {
        timeinfo->tm_mday -= 1;
    }
    follow_date = mktime(timeinfo);

    nftw(root_path,
         file_info,
         fd_limit, flags);
}

int main(int argc, char *argv[])
{

    if (argc != 6)
    {
        fprintf(stderr, "wrong arguments\n");
        exit(-1);
    }
    char *dir = argv[1];
    char *comman = argv[2];
    char *mod = argv[3];
    int day = atoi(argv[4]);
    int mdep = atoi(argv[5]);

    print_files_handler(dir, comman, mod[0], day, mdep);

    return 0;
}