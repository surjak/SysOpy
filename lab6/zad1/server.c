#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "keygen.h"
#include "message.h"
#include "types.h"
#include "queue.h"

#define MAX_CLIENTS 128

int clients[MAX_CLIENTS];
int queue;
int num_clients;

void clean()
{
    delete_queue(queue, get_public_key());
}

void empty_clients()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i] = -1;
    }
}

int main()
{
    empty_clients();
    atexit(clean);

    return 0;
}