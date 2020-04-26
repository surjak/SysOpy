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

void send_order()
{
    sembuf *load = calloc(4, sizeof(sembuf));

    load[0].sem_num = 0;
    load[0].sem_op = 0;
    load[0].sem_flg = 0;

    load[1].sem_num = 0;
    load[1].sem_op = 1;
    load[1].sem_flg = 0;

    load[2].sem_num = 4;
    load[2].sem_op = 1;
    load[2].sem_flg = 0;

    load[3].sem_num = 5;
    load[3].sem_op = -1;
    load[3].sem_flg = 0;

    semop(semaphore_id, load, 4);

    orders *ord = shmat(shared_memory_id, NULL, 0);

    int index = (semctl(semaphore_id, 4, GETVAL, NULL) - 1) % MAX_ORDERS;
    ord->values[index] *= 3;
    int orders_to_prepare = semctl(semaphore_id, 3, GETVAL, NULL);
    int orders_to_send = semctl(semaphore_id, 5, GETVAL, NULL);
    printf("[%d %ld] Wyslalem zamowienie o wielkosci: %d. Liczba zamowien do przygotowania: %d. Liczba zamowien do wyslania: %d.\n",
           getpid(), time(NULL), ord->values[index], orders_to_prepare, orders_to_send);
    ord->values[index] = 0;

    shmdt(ord);
    sembuf *back = calloc(1, sizeof(sembuf));

    back[0].sem_num = 0;
    back[0].sem_op = -1;
    back[0].sem_flg = 0;

    semop(semaphore_id, back, 1);
}

int main()
{
    srand(time(NULL));

    semaphore_id = get_semaphore();
    shared_memory_id = get_shared_memory();

    while (1)
    {
        usleep(rand_time);
        if (semctl(semaphore_id, 5, GETVAL, NULL) > 0)
        {
            send_order();
        }
    }

    return 0;
}