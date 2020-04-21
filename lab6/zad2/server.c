#include "types.h"
#include <signal.h>
#define MAX_CLIENTS 32
Client *clients[MAX_CLIENTS];

int server_queue = -1;
int clients_connected = 0;
int waiting = 0;
int current = 0;

void on_exit_server()
{
    printf("Server exits...\n");
    close_queue(server_queue);
    delete_queue(SERVER_NAME);

    exit(EXIT_SUCCESS);
}

void handle_sigint(int signal)
{
    char msg[MAX_MESSAGE_SIZE];
    sprintf(msg, "%d", SERVER_CLIENT_TERMINATE);

    if (clients_connected <= 0)
        on_exit_server();

    waiting = 1;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i])
        {
            send_message(clients[i]->queueDesc, msg, SERVER_CLIENT_TERMINATE);
        }
    }

    printf("Server is waiting for clients to terminate. Press Ctrl + C on each client side\n");
}

void handle_stop(char *msg)
{
    int type, clientId;
    sscanf(msg, "%d %d", &type, &clientId);

    clients[clientId] = NULL;
    free(clients[clientId]);

    clients_connected -= 1;
    printf("Server -- Received STOP from %d\n", clientId);

    if (waiting && clients_connected <= 0)
    {
        on_exit_server();
    }
}

void handle_disconnect(char *msg)
{
    int type, clientId;
    sscanf(msg, "%d %d", &type, &clientId);

    clients[clientId]->available = 1;
    printf("Server -- client with id %d left chat\n",
           clientId);
}

void handle_list(char *msg)
{
    int type, clientId;
    sscanf(msg, "%d %d", &type, &clientId);

    printf("Server -- listing clients \n");
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (i == clientId)
        {
            printf("\tClient --> id - %d (ME)\n", clients[i]->clientId);
        }
        else if (clients[i] && clients[i]->available)
        {
            printf("\tClient --> id - %d is available\n", clients[i]->clientId);
        }
        else if (clients[i] && !clients[i]->available)
        {
            printf("\tClient --> id - %d isn't available\n", clients[i]->clientId);
        }
    }
}

void handle_connect(char *msg)
{
    int type, clientId, chateeId;
    sscanf(msg, "%d %d %d", &type, &clientId, &chateeId);

    int id1 = clientId;
    int id2 = chateeId;

    if (id2 < 0 || id2 >= MAX_CLIENTS || !clients[id2] ||
        !clients[id2]->available || id1 == id2)
    {
        printf("Server -- client is not avaiable\n");
        return;
    }
    char msg1[MAX_MESSAGE_SIZE];
    char msg2[MAX_MESSAGE_SIZE];

    sprintf(msg1, "%d %d %s", SERVER_CLIENT_CHAT_INIT, id2, clients[id2]->name);
    sprintf(msg2, "%d %d %s", SERVER_CLIENT_CHAT_INIT, id1, clients[id1]->name);

    clients[id1]->available = 0;
    clients[id2]->available = 0;

    send_message(clients[id1]->queueDesc, msg1, SERVER_CLIENT_CHAT_INIT);
    send_message(clients[id2]->queueDesc, msg2, SERVER_CLIENT_CHAT_INIT);

    printf("Server -- initialized chat between %d and %d\n", id1, id2);
}

void handle_init(char *msg)
{
    int type;
    char *name = malloc(MAX_MESSAGE_SIZE * sizeof(char));
    sscanf(msg, "%d %s", &type, name);

    int pointer = -1;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        pointer = (current + i) % MAX_CLIENTS;
        if (!clients[pointer])
        {
            break;
        }
    }

    if (pointer == -1)
    {
        printf("Server is full, cannot add another client.\n");
    }
    else
    {
        Client *client = malloc(sizeof(Client));
        client->available = 1;
        client->name = name;
        client->clientId = pointer;
        client->queueDesc = get_queue(name);

        if (client->queueDesc == -1)
        {
            printError();
            printf("Failed to open client's queue\n");
        }

        clients[pointer] = client;

        char scMsg[MAX_MESSAGE_SIZE];
        sprintf(scMsg, "%d %d", SERVER_CLIENT_REGISTRED, pointer);

        send_message(client->queueDesc, scMsg, SERVER_CLIENT_REGISTRED);
        clients_connected += 1;
        printf("Server -- registered client - id: %d, queue_name: %s\n", client->clientId,
               client->name);
    }
}

void handle_message()
{
    char *msg = malloc(sizeof(char) * MAX_MESSAGE_SIZE);
    unsigned int type;
    receive_message(server_queue, msg, &type);

    if (type == CLIENT_SERVER_STOP)
    {
        handle_stop(msg);
    }
    else if (type == CLIENT_SERVER_DISCONNECT)
    {
        handle_disconnect(msg);
    }
    else if (type == CLIENT_SERVER_LIST)
    {
        handle_list(msg);
    }
    else if (type == CLIENT_SERVER_CONNECT)
    {
        handle_connect(msg);
    }
    else if (type == CLIENT_SERVER_INIT)
    {
        handle_init(msg);
    }
    else
    {
        printf("Unknown type\n");
    }

    free(msg);
}

int main(int argc, char *arrgv[])
{
    server_queue = create_queue(SERVER_NAME);
    if (server_queue == -1)
    {
        printf("Failed to open\n");
        printError();
    }
    signal(SIGINT, handle_sigint);

    printf("Server running...\n");
    while (1)
    {
        handle_message();
    }

    return 0;
}