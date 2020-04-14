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

int num_clients = 0;
int clients_tab_size = 0;

int stops = 0;
void clean()
{
    delete_queue(queue, get_public_key());
}

void send_private(int client_id, message *message)
{
    if (client_id >= clients_tab_size || clients[client_id] == -1)
    {
        fprintf(stderr, "wrong client\n");
        return;
    }
    if (send(clients[client_id], message) == -1)
    {
        perror("unable to send private message");
    }
}

void handle_sigint(int sig)
{
    printf("Server received SIGINT\n");
    stops = 1;
    if (num_clients == 0)
    {
        exit(0);
    }
    message message;
    message.type = TYPE_STOP;
    for (int i = 0; i < clients_tab_size; i++)
    {
        if (clients[i] != -1)
        {
            send_private(i, &message);
        }
    }
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
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);
    while (1)
    {
    }
    return 0;
}