#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "shared.h"
#define W_1 3
#define W_2 3
#define W_3 3
pid_t pids[W_1 + W_2 + W_3];

void clear()
{
    for (int i = 0; i < 6; i++)
        if (sem_unlink(SEMAPHORES[i]) < 0)
        {
            printf("Cant unline semaphore\n");
            printf("Errno: %d\n", errno);
            exit(EXIT_FAILURE);
        }
    if (shm_unlink(SHARED_MEMORY))
    {
        printf("Cant delete shared memory\n");
        printf("Errno: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    system("make clean");
}

void handle_SIGINT(int signum)
{
    printf("Konczenie dzialania...\n");
    for (int i = 0; i < W_1 + W_2 + W_3; i++)
    {
        kill(pids[i], SIGINT);
    }
    clear();
    exit(0);
}

void create_semaphores()
{
    sem_t *sem = sem_open(SEMAPHORES[0], O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);
    if (sem == SEM_FAILED)
    {
        printf("Cant cretate first semaphore\n");
        printf("Errno: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    sem_close(sem);

    for (int i = 1; i < 6; i++)
    {
        sem = sem_open(SEMAPHORES[i], O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 0);
        if (sem == SEM_FAILED)
        {
            printf("Cant create semaphoree\n");
            printf("Errno: %d\n", errno);
            exit(EXIT_FAILURE);
        }
        sem_close(sem);
    }
}
void create_shared_memory()
{
    int fd = shm_open(SHARED_MEMORY, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd < 0)
    {
        printf("Cant create shared memory\n");
        printf("Errno: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, sizeof(orders)) < 0)
    {
        printf("Cant create segment size\n");
        printf("Errno: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

void run_workers()
{
    for (int i = 0; i < W_1; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            execlp("./worker1", "worker1", NULL);
        }
        pids[i] = child_pid;
    }

    for (int i = 0; i < W_2; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            execlp("./worker2", "worker2", NULL);
        }
        pids[i + W_1] = child_pid;
    }

    for (int i = 0; i < W_3; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            execlp("./worker3", "worker3", NULL);
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
    create_semaphores();
    create_shared_memory();
    run_workers();
    clear();

    return 0;
}