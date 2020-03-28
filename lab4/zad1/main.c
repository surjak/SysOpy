#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int stop = 0;

void handle_sigint(int sig_no)
{
    printf("Otrzyno sygnal SIGINT.\n");
    exit(0);
}

void handle_sigtstp(int sig_no)
{
    if (stop)
    {
        stop = 0;
        return;
    }

    printf("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu. \n");
    stop = 1;
}

int main()
{
    signal(SIGINT, handle_sigint);
    struct sigaction act;
    act.sa_handler = handle_sigtstp;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGTSTP, &act, NULL);

    while (1)
    {

        if (!stop)
        {
            system("ls -la");
            sleep(1);
        }
        else
        {
            pause();
        }
    }
    return 0;
}
