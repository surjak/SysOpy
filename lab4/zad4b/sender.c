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
int catcher_PID;

int sigs_to_send;
int received_signals;
int num_send = 0;
union sigval value;
void send_next_signal()
{
    num_send++;
    if (mode == KILL || mode == SIGRT)
    {
        kill(catcher_PID, COUNT_SIGNAL);
    }
    else
    {
        sigqueue(catcher_PID, COUNT_SIGNAL, value);
    }
}

void send_end_signal()
{
    if (mode == KILL || mode == SIGRT)
    {
        kill(catcher_PID, END_SIGNAL);
    }
    else
    {
        sigqueue(catcher_PID, END_SIGNAL, value);
    }
}
void handle_signals(int sig, siginfo_t *info, void *ucontext)
{
    if (sig == COUNT_SIGNAL)
    {
        received_signals++;
        if (num_send < sigs_to_send)
        {
            send_next_signal();
        }
        else
        {
            send_end_signal();
        }
    }
    else if (sig == END_SIGNAL)
    {
        printf("sender received: %d signals, should receive: %d\n", received_signals, sigs_to_send);
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "wrong arguments, usage: [catcher PID] [num sig to send] [mode]\n");
    }
    catcher_PID = atoi(argv[1]);
    sigs_to_send = atoi(argv[2]);

    char *mode_str = argv[3];

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
    printf("created sender with PID: %d\n", getpid());
    send_next_signal();

    while (1)
    {
        usleep(100);
    }

    return 0;
}