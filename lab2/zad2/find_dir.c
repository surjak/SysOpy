#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

const char format[] = "%Y-%m-%d %H:%M:%S";

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

    printf("%s || type: %s, size: %ld, modification time: %s, access time: %s, nlinks: %ld\n",
           filename, file_type, statptr->st_size, modif_time_str, access_time_str, statptr->st_nlink);
}

void maxdepth(char *root_path, int depth)
{
    if (depth == 0)
        return;
    if (root_path == NULL)
        return;
    DIR *dir = opendir(root_path);

    if (dir == NULL)
    {
        fprintf(stderr, "error open dir: %s\n", strerror(errno));
        exit(-1);
    }

    struct dirent *file;

    char new_path[256];
    while ((file = readdir(dir)) != NULL)
    {
        strcpy(new_path, root_path);
        strcat(new_path, "/");
        strcat(new_path, file->d_name);

        struct stat sb;

        if (lstat(new_path, &sb) < 0)
        {
            fprintf(stderr, "unable to lstat file %s: %s\n", new_path, strerror(errno));
            exit(-1);
        }

        if (S_ISDIR(sb.st_mode))
        {
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
            {
                continue;
            }

            maxdepth(new_path, depth - 1);
        }

        print_from_stat(new_path, &sb);
    }
    closedir(dir);
}

void mtime(char *root_path, char mode, int count, time_t date)
{
    if (count == 0)
        return;
    if (root_path == NULL)
        return;
    DIR *dir = opendir(root_path);

    if (dir == NULL)
    {
        fprintf(stderr, "error open dir: %s\n", strerror(errno));
        exit(-1);
    }

    struct dirent *file;

    char new_path[256];
    while ((file = readdir(dir)) != NULL)
    {
        strcpy(new_path, root_path);
        strcat(new_path, "/");
        strcat(new_path, file->d_name);

        struct stat sb;

        if (lstat(new_path, &sb) < 0)
        {
            fprintf(stderr, "unable to lstat file %s: %s\n", new_path, strerror(errno));
            exit(-1);
        }

        if (S_ISDIR(sb.st_mode))
        {
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
            {
                continue;
            }

            mtime(new_path, mode, count - 1, date);
        }
        time_t modif_time = sb.st_mtime;
        if (mode == '-')
        {
            int diff_modif = difftime(date, modif_time);
            if (!((diff_modif == 0 && mode == '=') || (diff_modif > 0 && mode == '+') || (diff_modif < 0 && mode == '-')))
                continue;

            print_from_stat(new_path, &sb);
        }
        else if (mode == '+')
        {
            int diff_modif = difftime(date, modif_time);
            if (!((diff_modif == 0 && mode == '=') || (diff_modif > 0 && mode == '+') || (diff_modif < 0 && mode == '-')))
                continue;

            print_from_stat(new_path, &sb);
        }
        else if (mode == '=')
        {

            int diff_modif = abs(difftime(modif_time, date)) / (60 * 60 * 24);
            int diff_modif2 = difftime(date, modif_time);

            if ((diff_modif == 0 && mode == '=') && !(diff_modif2 < 0 && mode == '='))
                print_from_stat(new_path, &sb);
        }
    }
    closedir(dir);
}

void atime(char *root_path, char mode, int count, time_t date)
{
    if (count == 0)
        return;
    if (root_path == NULL)
        return;
    DIR *dir = opendir(root_path);

    if (dir == NULL)
    {
        fprintf(stderr, "error open dir: %s\n", strerror(errno));
        exit(-1);
    }

    struct dirent *file;

    char new_path[256];
    while ((file = readdir(dir)) != NULL)
    {
        strcpy(new_path, root_path);
        strcat(new_path, "/");
        strcat(new_path, file->d_name);

        struct stat sb;

        if (lstat(new_path, &sb) < 0)
        {
            fprintf(stderr, "unable to lstat file %s: %s\n", new_path, strerror(errno));
            exit(-1);
        }

        if (S_ISDIR(sb.st_mode))
        {
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
            {
                continue;
            }

            atime(new_path, mode, count - 1, date);
        }
        time_t modif_time = sb.st_atime;
        if (mode == '-')
        {
            int diff_modif = difftime(date, modif_time);
            if (!((diff_modif == 0 && mode == '=') || (diff_modif > 0 && mode == '+') || (diff_modif < 0 && mode == '-')))
                continue;

            print_from_stat(new_path, &sb);
        }
        else if (mode == '+')
        {
            int diff_modif = difftime(date, modif_time);
            if (!((diff_modif == 0 && mode == '=') || (diff_modif > 0 && mode == '+') || (diff_modif < 0 && mode == '-')))
                continue;

            print_from_stat(new_path, &sb);
        }
        else if (mode == '=')
        {

            int diff_modif = abs(difftime(modif_time, date)) / (60 * 60 * 24);
            int diff_modif2 = difftime(date, modif_time);

            if ((diff_modif == 0 && mode == '=') && !(diff_modif2 < 0 && mode == '='))
                print_from_stat(new_path, &sb);
        }
    }
    closedir(dir);
}

void print_files_handler(char *root_path, char *command, char mode, int count, int maxdep)
{

    if (strcmp(command, "maxdepth") == 0)
    {
        maxdepth(root_path, count);
    }
    else if (strcmp(command, "mtime") == 0)
    {
        time_t rawtime;
        struct tm *timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        timeinfo->tm_mday -= count;
        if (mode == '+')
        {
            timeinfo->tm_mday -= 1;
        }

        if (maxdep >= 0)
        {
            mtime(root_path, mode, maxdep, mktime(timeinfo));
        }
        else
        {
            mtime(root_path, mode, -1, mktime(timeinfo));
        }
    }
    else if (strcmp(command, "atime") == 0)
    {
        time_t rawtime;
        struct tm *timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        timeinfo->tm_mday -= count;
        if (mode == '+')
        {
            timeinfo->tm_mday -= 1;
        }
        if (maxdep >= 0)
        {
            atime(root_path, mode, maxdep, mktime(timeinfo));
        }
        else
        {
            atime(root_path, mode, -1, mktime(timeinfo));
        }
    }
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