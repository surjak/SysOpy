#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

const char format[] = "%Y-%m-%d %H:%M:%S";

void follow_dir()
{
    DIR *root_dir = opendir(".");
    if (root_dir == NULL)
    {
        fprintf(stderr, "Cant open directory, errno: %s\n", strerror(errno));
        exit(-1);
    }

    struct dirent *file;

    while ((file = readdir(root_dir)) != NULL)
    {
        struct stat sb;
        if (lstat(file->d_name, &sb) < 0)
        {
            fprintf(stderr, "can't lstat file %s, errno: %s\n", file->d_name, strerror(errno));
            exit(-1);
        }
        if (S_ISDIR(sb.st_mode))
        {
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
            {
                continue;
            }
            if (chdir(file->d_name) != 0)
            {
                fprintf(stderr, "unable to change path to %s", file->d_name);
                exit(-1);
            }

            follow_dir();
            pid_t pid_fork = fork();
            if (pid_fork < 0)
            {
                fprintf(stderr, "cant fork, errno: %s", strerror(errno));
                exit(-1);
            }
            else if (pid_fork == 0)
            {
                char cwd[4096];
                if (getcwd(cwd, 4096) == NULL)
                {
                    fprintf(stderr, "unable to get current working directory, errno: %s", strerror(errno));
                    exit(-1);
                }
                printf("dir: %s, PID: %d\n", cwd, getpid());
                int exec_status = execlp("ls", "ls", "-l", NULL);
                if (exec_status != 0)
                {
                    fprintf(stderr, "exec failed, errno: %s\n", strerror(errno));
                    exit(-1);
                }
                exit(exec_status);
            }
            else
            {
                wait(0);
            }
            if (chdir("..") != 0)
            {
                fprintf(stderr, "unable to change path to %s/../, errno: %s", file->d_name, strerror(errno));
                exit(-1);
            }
        }
        // printf("%s\n", file->d_name);
    }
    closedir(root_dir);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "wrong arguments\n");
        exit(-1);
    }

    if (chdir(argv[1]) != 0)
    {
        fprintf(stderr, "cant chmod to %s\n", argv[1]);
        exit(-1);
    }

    follow_dir();
    return 0;
}