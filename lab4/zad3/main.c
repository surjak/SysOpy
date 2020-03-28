#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

void handler_CHILD(int sig, siginfo_t *info, void *ucontext)
{
    printf("Signal number %d\n", info->si_signo);
    printf("Sending process ID %d\n", info->si_pid);
    printf("Child exit value %d\n", info->si_status);
}

void handle_SIGINT(int sig, siginfo_t *info, void *ucontext)
{
    printf("Signal number %d\n", info->si_signo);
    printf("Sending process ID %d\n", info->si_pid);
    if (info->si_code == SI_USER)
    {
        printf("USER\n");
    }
    else if (info->si_code == SI_KERNEL)
    {
        printf("KERNEL\n");
    }
    else if (info->si_code == SI_QUEUE)
    {
        printf("SIGQUEUE\n");
    }
}
void handle_QUEUE(int sig, siginfo_t *info, void *ucontext)
{
    printf("Signal number %d\n", info->si_signo);
    printf("Sending process ID %d\n", info->si_pid);
    printf("Value %d\n", info->si_value.sival_int);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("./main [child/status/queue]");
        exit(-1);
    }

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    if (strcmp(argv[1], "child") == 0)
    {
        act.sa_sigaction = handler_CHILD;
        sigaction(SIGCHLD, &act, NULL);
        pid_t child_pid = fork();

        if (child_pid == 0)
        {
            exit(123);
        }

        wait(NULL);
    }
    else if (strcmp(argv[1], "queue") == 0)
    {
        act.sa_sigaction = handle_QUEUE;
        sigaction(SIGINT, &act, NULL);

        union sigval sigval;
        sigval.sival_int = 123;

        sigqueue(getpid(), SIGINT, sigval);
    }
    else if (strcmp(argv[1], "sigint") == 0)
    {

        act.sa_sigaction = handle_SIGINT;
        sigaction(SIGINT, &act, NULL);
        union sigval sigval;

        sigqueue(getpid(), SIGINT, sigval);
    }

    return 0;
}