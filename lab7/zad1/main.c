#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "shared.h"

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

pid_t child_pids[W_1 + W_2 + W_3];

int sem_id;
int shm_id;

void clear()
{
    semctl(sem_id, 0, IPC_RMID, NULL);
    shmctl(shm_id, IPC_RMID, NULL);
    system("make clean");
}

void sigint_handle(int signum)
{
    printf("Konczenie dzialania...\n");
    for (int i = 0; i < W_1 + W_2 + W_3; i++)
    {
        kill(child_pids[i], SIGINT);
    }
    clear();
    exit(0);
}

int main()
{
    printf("HELLO");
    signal(SIGINT, sigint_handle);
    key_t sem_key = ftok(getenv("HOME"), 0);
    sem_id = semget(sem_key, 6, IPC_CREAT | 0666);
    if (sem_id < 0)
    {
        printf("Cannot create semafors set %d\n", errno);
        exit(EXIT_FAILURE);
    }

    union semun arg;
    arg.val = 0;

    for (int i = 0; i < 6; i++)
    {
        semctl(sem_id, i, SETVAL, arg);
    }
    key_t shm_key = ftok(getenv("HOME"), 1);
    shm_id = shmget(shm_key, sizeof(orders), IPC_CREAT | 0666);
    if (shm_id < 0)
    {
        printf("Cannot create shared memory %d\n", errno);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < W_1; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            execlp("./worker_1", "worker_1", NULL);
        }
        child_pids[i] = child_pid;
    }

    for (int i = 0; i < W_2; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            execlp("./worker_2", "worker_2", NULL);
        }
        child_pids[i + W_1] = child_pid;
    }

    for (int i = 0; i < W_3; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            execlp("./worker_3", "worker_3", NULL);
        }
        child_pids[i + W_1 + W_2] = child_pid;
    }
    for (int i = 0; i < W_1 + W_2 + W_3; i++)
    {
        wait(NULL);
    }

    clear();
    return 0;
}