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
int clients_friends[MAX_CLIENTS][2];

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
        clients_friends[cl_id][0] = cl_id;

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
        for (int j = 0; j < 2; ++j)
        {
            clients_friends[i][j] = -1;
        }
    }
}

void handle_stop(message_t *message)
{
    int client_id = message->id;
    if (close_queue(clients[client_id]) == -1)
    {
        perror("cant close client queue\n");
    }
    clients[client_id] = -1;
    num_clients--;
    printf("client: %d stops, clients left: %d\n", client_id, num_clients);
    if (num_clients == 0 && stops == 1)
    {
        exit(0);
    }
}
void handle_list(message_t *message)
{
    int client_id = message->id;
    printf("Server - handle list\n");
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients_friends[i][0] >= 0)
        {
            if (i == client_id)
            {
                printf("It's you: %d   ", i);
            }
            if (clients_friends[i][1] == -1)
            {
                printf("Client %d is available to connect\n", i);
            }
            else
            {
                printf("Client %d make a conversation right now\n", i);
            }
        }
    }
}

void handle_connect(message_t *message)
{
    int client_id = message->id;
    int connect_id;

    sscanf(message->text, "%d", &connect_id);
    if (client_id == connect_id)
    {
        printf("You can't connect with youself\n");
        message_t mess;
        mess.type = TYPE_CONNECT;
        sprintf(mess.text, "%d", -1);
        send_private(client_id, &mess);
        printf("Can't connect to client: %d\n", connect_id);
        return;
    }
    printf("Received from %d, wants to connect %d\n", client_id, connect_id);

    message_t mess;
    mess.type = TYPE_CONNECT;
    if (clients[connect_id] != -1 && clients_friends[connect_id][1] == -1)
    {
        sprintf(mess.text, "%d", clients[connect_id]);
    }
    else
    {
        sprintf(mess.text, "%d", -1);
        send_private(client_id, &mess);
        printf("Can't connect to client: %d\n", connect_id);
        return;
    }
    clients_friends[client_id][1] = connect_id;
    clients_friends[connect_id][1] = client_id;

    send_private(client_id, &mess);
    sprintf(mess.text, "%d", clients[client_id]);
    send_private(connect_id, &mess);
    printf("sent connect confirmation for client: %d\n", client_id);
    printf("sent connect confirmation for client: %d\n", connect_id);
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
        case TYPE_STOP:
        {
            handle_stop(&message);
            break;
        }
        case TYPE_LIST:
        {
            handle_list(&message);
            break;
        }
        case TYPE_CONNECT:
        {
            handle_connect(&message);
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