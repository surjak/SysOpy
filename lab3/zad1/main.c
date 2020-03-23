#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>

void search_dir(char *path)
{
    if (!path)
        return;
    DIR *directory = opendir(path);
    if (!directory)
        return;
    struct dirent *d;
    struct stat st;

    while ((d = readdir(directory)))
    {
        if (strcmp(d->d_name, "..") == 0)
            continue;
        if (strcmp(d->d_name, ".") == 0)
            continue;
        char *dir = malloc(sizeof(char) * (strlen(path) + strlen(d->d_name)) + 2);
        sprintf(dir, "%s/%s", path, d->d_name);
        if (lstat(dir, &st) < 0)
            continue;
        if (S_ISDIR(st.st_mode) && fork() == 0)
        {
            printf("\npid(%i) path(%s)\n", getpid(), dir);
            static const char cmd[] = "ls -la %s";
            char *ls = calloc(sizeof(cmd), sizeof(char));
            sprintf(ls, cmd, dir);
            system(ls);
            free(ls);
            exit(0);
        }
        else
            wait(NULL);
        free(dir);
    }
    closedir(directory);
}

int main(int argc, char **argv)
{
    char *path = argc > 1 ? argv[1] : ".";
    search_dir(path);
    return 0;
}