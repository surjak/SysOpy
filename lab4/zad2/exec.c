#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (strcmp(argv[1], "pending") != 0)
    {
        raise(SIGUSR1);
    }
    if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0)
    {
        sigset_t newmask;
        sigpending(&newmask);
        printf("Signal pending: %d\n", sigismember(&newmask, SIGUSR1));
    }
    return 0;
}