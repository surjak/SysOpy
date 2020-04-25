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
#include <time.h>

#include "shared.h"

const int MIN_VAL = 1;
const int MAX_VAL = 100;
int sem_id;
int shm_id;

typedef struct sembuf sembuf;

int rand_int()
{
    return rand() % (MAX_VAL - MIN_VAL + 1) + MIN_VAL;
}

int rand_time()
{
    return (rand() % (max_sleep - min_sleep + 1) + min_sleep) * 1000;
}

void pack_order_init()
{
    sembuf *sops = calloc(4, sizeof(sembuf));

    sops[0].sem_num = 0;
    sops[0].sem_op = 0;
    sops[0].sem_flg = 0;

    sops[1].sem_num = 0;
    sops[1].sem_op = 1;
    sops[1].sem_flg = 0;

    sops[2].sem_num = 2;
    sops[2].sem_op = 1;
    sops[2].sem_flg = 0;

    sops[3].sem_num = 3;
    sops[3].sem_op = -1;
    sops[3].sem_flg = 0;

    semop(sem_id, sops, 4);
}

void pack_order()
{
    orders *ord = shmat(shm_id, NULL, 0);

    int index = (semctl(sem_id, 2, GETVAL, NULL) - 1) % MAX_ORDERS;
    ord->values[index] *= 2;
    printf("[%d %ld] Przygotowalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n",
           getpid(), time(NULL), ord->values[index], semctl(sem_id, 3, GETVAL, NULL), semctl(sem_id, 5, GETVAL, NULL) + 1);

    shmdt(ord);
}

void pack_order_finalize()
{
    sembuf *sops = (sembuf *)calloc(2, sizeof(sembuf));

    sops[0].sem_num = 0;
    sops[0].sem_op = -1;
    sops[0].sem_flg = 0;

    sops[1].sem_num = 5;
    sops[1].sem_op = 1;
    sops[1].sem_flg = 0;

    semop(sem_id, sops, 2);
}

int main()
{
    srand(time(NULL));

    key_t sem_key = ftok(getenv("HOME"), 0);
    sem_id = semget(sem_key, 0, 0);
    if (sem_id < 0)
    {
        printf("Can't get semafor %d\n", errno);
        exit(EXIT_FAILURE);
    }

    key_t shm_key = ftok(getenv("HOME"), 1);
    shm_id = shmget(shm_key, 0, 0);
    if (shm_id < 0)
    {
        printf("Can't access shared memory %d\n", errno);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        usleep(rand_time());
        if (semctl(sem_id, 3, GETVAL, NULL) > 0)
        {
            pack_order_init();
            pack_order();
            pack_order_finalize();
        }
    }

    return 0;
}