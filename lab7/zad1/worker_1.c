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

int semaphore_id;
int shared_memory_id;

typedef struct sembuf sembuf;

void add_order()
{
    sembuf *load = calloc(3, sizeof(sembuf));
    load[0].sem_num = 0;
    load[0].sem_op = 0;
    load[0].sem_flg = 0;

    load[1].sem_num = 0;
    load[1].sem_op = 1;
    load[1].sem_flg = 0;

    load[2].sem_num = 1;
    load[2].sem_op = 1;
    load[2].sem_flg = 0;

    semop(semaphore_id, load, 3);

    orders *ord = shmat(shared_memory_id, NULL, 0);
    int index = (semctl(semaphore_id, 1, GETVAL, NULL) - 1) % MAX_ORDERS;
    int value = rand_int;
    ord->values[index] = value;
    int orders_to_prepare = semctl(semaphore_id, 3, GETVAL, NULL) + 1;
    int orders_to_send = semctl(semaphore_id, 5, GETVAL, NULL);
    printf("[%d %ld] Dodalem liczbe: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n",
           getpid(), time(NULL), value, orders_to_prepare, orders_to_send);

    shmdt(ord);

    sembuf *back = calloc(2, sizeof(sembuf));

    back[0].sem_num = 0;
    back[0].sem_op = -1;
    back[0].sem_flg = 0;

    back[1].sem_num = 3;
    back[1].sem_op = 1;
    back[1].sem_flg = 0;

    semop(semaphore_id, back, 2);
}

int main()
{
    srand(time(NULL));

    semaphore_id = get_semaphore();
    shared_memory_id = get_shared_memory();

    while (1)
    {
        usleep(rand_time);
        if (semctl(semaphore_id, 3, GETVAL, NULL) + semctl(semaphore_id, 5, GETVAL, NULL) < MAX_ORDERS)
        {
            add_order();
        }
    }
}