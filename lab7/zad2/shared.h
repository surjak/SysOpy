#ifndef SHARED_H
#define SHARED_H

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
#define MAX_ORDERS 5
#define min_sleep 100
#define max_sleep 1000
#define MIN_VAL 1
#define MAX_VAL 100
typedef struct
{
    int values[MAX_ORDERS];
} orders;

#define rand_int (rand() % (MAX_VAL - MIN_VAL + 1) + MIN_VAL)

#define rand_time ((rand() % (max_sleep - min_sleep + 1) + min_sleep) * 1000)

#endif //SHARED_H