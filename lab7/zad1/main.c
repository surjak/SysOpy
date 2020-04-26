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

#define W_1 3
#define W_2 3
#define W_3 3

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

pid_t pids[W_1 + W_2 + W_3];

int semaphore_id;
int shared_memory_id;

void set_semaphore()
{
    key_t sem_key = ftok(getenv("HOME"), 0);
    semaphore_id = semget(sem_key, 6, IPC_CREAT | 0666);
    if (semaphore_id < 0)
    {
        printf("Cannot create semaphores set %d\n", errno);
        exit(EXIT_FAILURE);
    }
    union semun arg;
    arg.val = 0;

    for (int i = 0; i < 6; i++)
    {
        semctl(semaphore_id, i, SETVAL, arg);
    }
}

void create_shared_memory()
{
    key_t shm_key = ftok(getenv("HOME"), 1);
    shared_memory_id = shmget(shm_key, sizeof(orders), IPC_CREAT | 0666);
    if (shared_memory_id < 0)
    {
        printf("Cannot create shared memory %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

void clear()
{
    semctl(semaphore_id, 0, IPC_RMID, NULL);
    shmctl(shared_memory_id, IPC_RMID, NULL);
    system("make clean");
}

void handle_SIGINT(int signum)
{
    for (int i = 0; i < W_1 + W_2 + W_3; i++)
    {
        kill(pids[i], SIGINT);
    }
    clear();
    exit(0);
}

void run_workers()
{
    for (int i = 0; i < W_1; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            execlp("./worker_1", "worker_1", NULL);
        }
        pids[i] = child_pid;
    }

    for (int i = 0; i < W_2; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            execlp("./worker_2", "worker_2", NULL);
        }
        pids[i + W_1] = child_pid;
    }

    for (int i = 0; i < W_3; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            execlp("./worker_3", "worker_3", NULL);
        }
        pids[i + W_1 + W_2] = child_pid;
    }
    for (int i = 0; i < W_1 + W_2 + W_3; i++)
    {
        wait(NULL);
    }
}

int main()
{
    signal(SIGINT, handle_SIGINT);
    set_semaphore();
    create_shared_memory();
    run_workers();
    clear();
    return 0;
}