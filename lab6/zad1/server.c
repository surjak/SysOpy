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

int find_client_id()
{
    if (clients_tab_size < MAX_CLIENTS)
    {
        num_clients++;
        return clients_tab_size++;
    }
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] == -1)
        {
            num_clients++;
            return i;
        }
    }
    return -1;
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

void handle_init(message *mess)
{

    key_t key;
    sscanf(mess->text, "%d", &key);
    int client_id = find_client_id();
    printf("%d\n", key);
    if (client_id != -1)
    {
        if ((clients[client_id] = get_queue(key)) == -1)
        {
            perror("can't open client private queue");
        }
        message message;
        message.type = TYPE_INIT;
        message.id = client_id;
        send_private(client_id, &message);
        printf("sent register confirmation for client: %d\n", client_id);
    }
    else
    {
        fprintf(stderr, "clients full error\n");
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
    key_t key = get_public_key();
    if ((queue = create_queue(key)) == -1)
    {
        perror("cant create queue");
        exit(1);
    }

    message message;
    while (1)
    {
        if (receive(queue, &message) == -1)
        {
            printf("Can't receive \n");
            if (errno != EINTR)
            {
                perror("can't receive message");
                exit(1);
            }
            continue;
        }
        switch (message.type)
        {
        case TYPE_INIT:
            handle_init(&message);
            break;

        default:
            fprintf(stderr, "wrong type\n");
            break;
        }
    }
    return 0;
}