#include "types.h"
#include <signal.h>

#define MAX_CLIENTS 128

Client *clients[MAX_CLIENTS];

int server_queue = -1;
int clients_count = 0;
int waiting = 0;
int current = 0;

void on_exit_server()
{
    printf("Server exits\n");
    close_queue(server_queue);
    delete_queue(SERVER_NAME);
    exit(EXIT_SUCCESS);
}

void handle_exit(int sig)
{
    char mess[MAX_MESSAGE_LENGHT];
    sprintf(mess, "%d", SERVER_CLIENT_TERMINATE);
    if (clients_count <= 0)
        on_exit_server();
    waiting = 1;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i])
        {
            send_message(clients[i]->queue, mess, SERVER_CLIENT_TERMINATE);
        }
    }
    printf("Server -- waiting for clients to terminate.\n");
}

int get_client_id()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] == NULL)
            return i;
    }
    return -1;
}
void handle_init(char *msg)
{
    int type;
    char *name = malloc(MAX_MESSAGE_LENGHT * sizeof(char));
    sscanf(msg, "%d %s", &type, name);
    int pointer = get_client_id();
    if (pointer == -1)
    {
        printf("Server reached max clients \n");
        return;
    }
    Client *client = malloc(sizeof(Client));
    client->available = 1;
    client->name = name;
    client->id = pointer;
    client->queue = get_queue(name);
    if (client->queue == -1)
    {
        err("Can't open client queue on init \n");
    }
    clients[pointer] = client;
    char response[MAX_MESSAGE_LENGHT];
    sprintf(response, "%d %d", SERVER_CLIENT_REGISTRED, pointer);
    send_message(client->queue, response, SERVER_CLIENT_REGISTRED);
    clients_count++;
    printf("Server -- registered client - id: %d, path: %s\n", client->id,
           client->name);
}
void handle_list(char *msg)
{
    int type, client_id;
    sscanf(msg, "%d %d", &type, &client_id);
    printf("Server -- listing clients\n");
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (i == client_id)
        {
            printf("\tClient --> id - %d, name - %s (ME)\n", clients[i]->id,
                   clients[i]->name);
        }
        else if (clients[i] && clients[i]->available)
        {
            printf("\tClient --> id - %d, name - %s is available\n", clients[i]->id,
                   clients[i]->name);
        }
        else if (clients[i] && !clients[i]->available)
        {
            printf("\tClient --> id - %d, name - %s isn't available\n", clients[i]->id,
                   clients[i]->name);
        }
    }
}
void handle_stop(char *msg)
{
    int type, client_id;
    sscanf(msg, "%d %d", &type, &client_id);
    clients[client_id] = NULL;
    free(clients[client_id]);
    clients_count--;
    printf("Server -- Received STOP\n");
    if (waiting && clients_count <= 0)
    {
        on_exit_server();
    }
}
void handle_disconnect(char *msg)
{
    int type, client_id;
    sscanf(msg, "%d %d", &type, &client_id);
    clients[client_id]->available = 1;
    printf("Server -- client with id %d is now available..\n",
           client_id);
}
void handle_connect(char *msg)
{
    int type, id1, id2;
    sscanf(msg, "%d %d %d", &type, &id1, &id2);
    if (id2 < 0 || id2 >= MAX_CLIENTS || !clients[id2] || !clients[id2]->available || id1 == id2)
    {
        printf("Server -- cant connect\n");
        return;
    }
    char msg1[MAX_MESSAGE_LENGHT];
    char msg2[MAX_MESSAGE_LENGHT];
    sprintf(msg1, "%d %d %s", SERVER_CLIENT_CHAT_INIT, id2, clients[id2]->name);
    sprintf(msg2, "%d %d %s", SERVER_CLIENT_CHAT_INIT, id1, clients[id1]->name);
    clients[id1]->available = 0;
    clients[id2]->available = 0;
    send_message(clients[id1]->queue, msg1, SERVER_CLIENT_CHAT_INIT);
    send_message(clients[id2]->queue, msg2, SERVER_CLIENT_CHAT_INIT);
    printf("Server -- initialized chat, %d <=> %d\n", id1, id2);
}
void handle_message()
{
    char *msg = malloc(sizeof(char) * MAX_MESSAGE_LENGHT);
    unsigned int type;
    receive_message(server_queue, msg, &type);
    switch (type)
    {
    case CLIENT_SERVER_STOP:
        handle_stop(msg);
        break;
    case CLIENT_SERVER_DISCONNECT:
        handle_disconnect(msg);
        break;
    case CLIENT_SERVER_LIST:
        handle_list(msg);
        break;
    case CLIENT_SERVER_CONNECT:
        break;
    case CLIENT_SERVER_INIT:
        handle_init(msg);
        break;

    default:
        printf("Unknow command");
    }
    free(msg);
}
int main()
{
    server_queue = create_queue(SERVER_NAME);
    if (server_queue == -1)
        err("Cant create queue for server");
    printf("Running server... \n");
    signal(SIGINT, handle_exit);
    while (1)
        handle_message();
    return 0;
}