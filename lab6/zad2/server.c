#include "types.h"
#include <signal.h>

Client *clients[MAX_CLIENTS];

int serverQueueDesc = -1;
int clientsRunningCount = 0;
int waitingForClientsToTerminate = 0;
int current = 0;

// HANDLE EXIT - CRTL+C
void exitServer()
{
    printf("Server exits...\n");
    close_queue(serverQueueDesc);
    delete_queue(SERVER_NAME);

    exit(EXIT_SUCCESS);
}

void handleSignalExit(int signal)
{
    char msg[MAX_MSG_LENGTH];
    sprintf(msg, "%d", SERVER_CLIENT_TERMINATE);

    if (clientsRunningCount <= 0)
        exitServer();

    waitingForClientsToTerminate = 1;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i])
        {
            send_message(clients[i]->queueDesc, msg, SERVER_CLIENT_TERMINATE);
        }
    }

    printf("Server -- waiting for clients to terminate.\n");
}
// -------------------------

// HANDLE - STOP
void handleStop(char *msg)
{
    int type, clientId;
    sscanf(msg, "%d %d", &type, &clientId);

    clients[clientId] = NULL;
    free(clients[clientId]);

    clientsRunningCount -= 1;
    printf("Server -- Received STOP from %d\n", clientId);

    if (waitingForClientsToTerminate && clientsRunningCount <= 0)
    {
        exitServer();
    }
}
// -------------------------

// HANDLE - DISCONNECT
void handleDisconnect(char *msg)
{
    int type, clientId;
    sscanf(msg, "%d %d", &type, &clientId);

    clients[clientId]->available = 1;
    printf("Server -- client with id %d left chat and is now available..\n",
           clientId);
}
// -------------------------

// HANDLE - LIST
void handleList(char *msg)
{
    int type, clientId;
    sscanf(msg, "%d %d", &type, &clientId);

    printf("Server -- listing available clients as requested by client with id "
           "%d...\n",
           clientId);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (i == clientId)
        {
            printf("\tClient --> id - %d, path - %s (ME)\n", clients[i]->clientId,
                   clients[i]->name);
        }
        else if (clients[i] && clients[i]->available)
        {
            printf("\tClient --> id - %d, path - %s\n", clients[i]->clientId,
                   clients[i]->name);
        }
    }
}
// -------------------------

// HANDLE - CONNECT
void handleConnect(char *msg)
{
    int type, clientId, chateeId;
    sscanf(msg, "%d %d %d", &type, &clientId, &chateeId);

    int id1 = clientId;
    int id2 = chateeId;

    // Check if client under sent id is avaiable
    if (id2 < 0 || id2 >= MAX_CLIENTS || !clients[id2] ||
        !clients[id2]->available || id1 == id2)
    {
        printf("Server -- requested client is not avaiable\n");
        return;
    }
    char msg1[MAX_MSG_LENGTH];
    char msg2[MAX_MSG_LENGTH];

    sprintf(msg1, "%d %d %s", SERVER_CLIENT_CHAT_INIT, id2, clients[id2]->name);
    sprintf(msg2, "%d %d %s", SERVER_CLIENT_CHAT_INIT, id1, clients[id1]->name);

    clients[id1]->available = 0;
    clients[id2]->available = 0;

    send_message(clients[id1]->queueDesc, msg1, SERVER_CLIENT_CHAT_INIT);
    send_message(clients[id2]->queueDesc, msg2, SERVER_CLIENT_CHAT_INIT);

    printf("Server -- initialized chat, %d <=> %d\n", id1, id2);
}
// -------------------------

// HANDLE - INIT
void handleInit(char *msg)
{
    int type;
    char *name = malloc(MAX_MSG_LENGTH * sizeof(char));
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
        printf("Server -- reached maximum capcity, cannot add another client...\n");
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
            printf("Faile to open client's queue\n");
        }

        clients[pointer] = client;

        // Notify client that he's now registered.
        char scMsg[MAX_MSG_LENGTH];
        sprintf(scMsg, "%d %d", SERVER_CLIENT_REGISTRED, pointer);

        send_message(client->queueDesc, scMsg, SERVER_CLIENT_REGISTRED);
        clientsRunningCount += 1;
        printf("Server -- registered client - id: %d, path: %s\n", client->clientId,
               client->name);
    }
}
// -------------------------

// RECEIVE MESSAGE
// Note that we handle messages in order based on priority.
void handleMessage()
{
    char *msg = malloc(sizeof(char) * MAX_MSG_LENGTH);
    unsigned int type;
    receive_message(serverQueueDesc, msg, &type);

    if (type == CLIENT_SERVER_STOP)
    {
        handleStop(msg);
    }
    else if (type == CLIENT_SERVER_DISCONNECT)
    {
        handleDisconnect(msg);
    }
    else if (type == CLIENT_SERVER_LIST)
    {
        handleList(msg);
    }
    else if (type == CLIENT_SERVER_CONNECT)
    {
        handleConnect(msg);
    }
    else if (type == CLIENT_SERVER_INIT)
    {
        handleInit(msg);
    }
    else
    {
        printf("Unknown type\n");
    }

    free(msg);
}

int main(int argc, char *arrgv[])
{
    serverQueueDesc = create_queue(SERVER_NAME);
    if (serverQueueDesc == -1)
    {
        printf("failed to open\n");
        printError();
    }
    signal(SIGINT, handleSignalExit);

    printf("Server running...\n");
    while (1)
    {
        handleMessage();
    }

    return 0;
}