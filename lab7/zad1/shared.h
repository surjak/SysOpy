#ifndef SHARED_H
#define SHARED_H

#define MAX_ORDERS 5
#define W_1 3
#define W_2 3
#define W_3 3

int min_sleep = 100;
int max_sleep = 1000;

const char *NAMES[6] = {"/MEMACCESS", "/FREEINDEX", "/PACKINDEX", "/PACKCOUNT", "/SENDINDEX", "/SENDCOUNT"};
const char *SHM_NAME = "/ORDERS";

typedef struct
{
    int values[MAX_ORDERS];
} orders;

#endif //SHARED_H