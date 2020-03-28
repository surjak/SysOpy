#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

typedef enum Mode
{
    KILL,
    QUEUE,
    SIGRT
} Mode;

int COUNT_SIGNAL;
int END_SIGNAL;

Mode mode;

int received_signals = 0;

void handle_signals(int sig, siginfo_t *info, void *ucontext)
{
    if (sig == COUNT_SIGNAL)
        received_signals++;
    else if (sig == END_SIGNAL)
    {
        if (mode == KILL || mode == SIGRT)
        {
            for (int i = 0; i < received_signals; ++i)
            {
                kill(info->si_pid, COUNT_SIGNAL);
            }
            kill(info->si_pid, END_SIGNAL);
        }
        else
        {
            union sigval value;
            for (int i = 0; i < received_signals; ++i)
            {
                value.sival_int = i;
                sigqueue(info->si_pid, COUNT_SIGNAL, value);
            }
            sigqueue(info->si_pid, END_SIGNAL, value);
        }
        printf("catcher received: %d signals\n", received_signals);
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "wrong arguments, usage: [mode]\n");
        exit(1);
    }

    char *mode_str = argv[1];

    if (strcmp("kill", mode_str) == 0)
    {
        mode = KILL;
        COUNT_SIGNAL = SIGUSR1;
        END_SIGNAL = SIGUSR2;
    }
    else if (strcmp("queue", mode_str) == 0)
    {
        mode = QUEUE;
        COUNT_SIGNAL = SIGUSR1;
        END_SIGNAL = SIGUSR2;
    }
    else if (strcmp("sigrt", mode_str) == 0)
    {
        mode = SIGRT;
        COUNT_SIGNAL = SIGRTMIN + 1;
        END_SIGNAL = SIGRTMIN + 2;
    }
    else
    {
        fprintf(stderr, "wring mode, allowed: [kill, queue, sigrt]\n");
        exit(1);
    }

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, COUNT_SIGNAL);
    sigdelset(&mask, END_SIGNAL);

    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
    {
        perror("cant block signals\n");
        exit(1);
    }

    struct sigaction sa_handle;
    sa_handle.sa_flags = SA_SIGINFO;
    sa_handle.sa_sigaction = handle_signals;

    sigemptyset(&sa_handle.sa_mask);
    sigaddset(&sa_handle.sa_mask, COUNT_SIGNAL);
    sigaddset(&sa_handle.sa_mask, END_SIGNAL);

    sigaction(COUNT_SIGNAL, &sa_handle, NULL);
    sigaction(END_SIGNAL, &sa_handle, NULL);

    printf("created catcher with PID: %d\n", getpid());

    while (1)
    {
        usleep(100);
    }

    return 0;
}