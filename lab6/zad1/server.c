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
#include "que.h"
#include "utils.h"

#define MAX_CLIENTS 128
#define MAX_GROUP 32

int queue;

int clients[MAX_CLIENTS];
int clients_friends[MAX_CLIENTS][MAX_CLIENTS];

int num_clients = 0;
int clients_tab_size = 0;

int stops = 0;

void clean()
{
    delete_queue(queue, get_public_key());
}

void send_private(unsigned int client_id, message_t *message)
{
    if (client_id >= clients_tab_size || clients[client_id] == -1)
    {
        fprintf(stderr, "wrong client!\n");
        return;
    }
    if ((send(clients[client_id], message) == -1))
    {
        perror("unable to send private message");
    }
}

void handle_sigint(int sig)
{
    stops = 1;
    if (num_clients == 0)
    {
        exit(0);
    }

    message_t message;
    message.type = TYPE_STOP;
    for (int i = 0; i < clients_tab_size; ++i)
    {
        if (clients[i] != -1)
        {
            send_private(i, &message);
        }
    }
}

int find_cliend_id()
{

    if (clients_tab_size < MAX_CLIENTS)
    {
        num_clients++;
        return clients_tab_size++;
    }
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i] == -1)
        {
            num_clients++;
            return i;
        }
    }

    return -1;
}

void handle_init(message_t *msg)
{
    key_t key;
    sscanf(msg->text, "%d", &key);

    int cl_id = find_cliend_id();
    if (cl_id != -1)
    {
        if ((clients[cl_id] = get_queue(key)) == -1)
        {
            perror("cant open client private queue");
        }

        message_t message;
        message.type = TYPE_INIT;
        message.id = cl_id;

        send_private(cl_id, &message);

        printf("sent register confirmation for client: %d\n", cl_id);
    }
    else
    {
        fprintf(stderr, "clients full error\n");
    }
}

void init()
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        clients[i] = -1;
        for (int j = 0; j < MAX_CLIENTS; ++j)
        {
            clients_friends[i][j] = 0;
        }
    }
}

int main(int argc, char *argv[])
{

    init();

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

    message_t message;

    while (1)
    {
        if (receive(queue, &message) == -1)
        {
            printf("cant receive\n");
            if (errno != EINTR)
            {
                perror("cant receive message");
                exit(1);
            }

            continue;
        }
        switch (message.type)
        {
        case TYPE_INIT:
        {
            handle_init(&message);
            break;
        }
        default:
        {
            fprintf(stderr, "wrong type\n");
            break;
        }
        }
    }

    return 0;
}