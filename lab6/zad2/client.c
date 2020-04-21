#include "types.h"
#include <signal.h>

int clientQueueDesc;
int serverQueueDesc;
int clientId;
char *name;

// When chatting
int chateeQueueDesc = -1;

void exitClient()
{
    printf("Client -- exit..\n");
    close_queue(serverQueueDesc);
    close_queue(clientQueueDesc);

    delete_queue(name);
    exit(EXIT_SUCCESS);
}

// SEND - INIT
void registerMe()
{
    char csMsg[MAX_MSG_LENGTH];
    sprintf(csMsg, "%d %s", CLIENT_SERVER_INIT, name);

    send_message(serverQueueDesc, csMsg, CLIENT_SERVER_INIT);

    char scMsg[MAX_MSG_LENGTH];
    unsigned int type;
    receive_message(clientQueueDesc, scMsg, &type);

    sscanf(scMsg, "%d %d", &type, &clientId);
    printf("Client -- registered with id: %d, key: %s\n", clientId, name);
}
// ----------------

// SEND - STOP
void sendStop()
{
    printf("Client -- sending STOP..\n");

    char csMsg[MAX_MSG_LENGTH];
    sprintf(csMsg, "%d %d", CLIENT_SERVER_STOP, clientId);
    send_message(serverQueueDesc, csMsg, CLIENT_SERVER_STOP);

    exitClient();
}

void handleExitSignal(int sig) { sendStop(); }
// ----------------

// SEND - LIST
void sendList()
{
    printf("Client -- sending LIST..\n");

    char csMsg[MAX_MSG_LENGTH];
    sprintf(csMsg, "%d %d", CLIENT_SERVER_LIST, clientId);
    send_message(serverQueueDesc, csMsg, CLIENT_SERVER_LIST);
}
// ----------------

// SEND - DISCONNECT
void sendDisconnect()
{
    printf("Client -- disconnected chat\n");

    char csMsg[MAX_MSG_LENGTH];
    sprintf(csMsg, "%d %d", CLIENT_SERVER_DISCONNECT, clientId);
    send_message(serverQueueDesc, csMsg, CLIENT_SERVER_DISCONNECT);

    if (chateeQueueDesc != -1)
    {
        char ccMsg[MAX_MSG_LENGTH];
        sprintf(csMsg, "%d %d", CLIENT_CLIENT_DICONNECT, clientId);
        send_message(chateeQueueDesc, ccMsg, CLIENT_CLIENT_DICONNECT);

        close_queue(chateeQueueDesc);
        chateeQueueDesc = -1;
    }
}
// ----------------

// SEND - CONNECT
void sendConnect(int chateeId)
{
    printf("Client -- seending CONNECT to chatee with ID: %d\n", chateeId);

    char csMsg[MAX_MSG_LENGTH];
    sprintf(csMsg, "%d %d %d", CLIENT_SERVER_CONNECT, clientId, chateeId);
    send_message(serverQueueDesc, csMsg, CLIENT_SERVER_CONNECT);
}
// ----------------

// SEND - MSG
void sendMessage(char *message)
{
    char ccMsg[MAX_MSG_LENGTH];
    sprintf(ccMsg, "%d %s", CLIENT_CLIENT_MSG, message);

    send_message(chateeQueueDesc, ccMsg, CLIENT_CLIENT_MSG);
    printf("------------------------------------------------\n");
    printf("Me:          \t%s\n", message);
    printf("------------------------------------------------\n");
}
// ----------------

// HANDLE - DISCONNECT
void handleDisconnect(char *msg)
{
    printf("Client -- received disconnect msg from chatee..\n");

    chateeQueueDesc = -1;
    close_queue(chateeQueueDesc);
    sendDisconnect();
}
// ----------------

// Handle - CHAT_INIT
void handleChatInit(char *msg)
{
    int chateeId;
    unsigned int type;
    char chateeName[MAX_MSG_LENGTH];
    sscanf(msg, "%d %d %s", &type, &chateeId, chateeName);
    printf("Client -- entering chat with %d..\n", chateeId);
    chateeQueueDesc = get_queue(chateeName);
}
// ----------------

// Handle - TERMINATE
void handleTerminate()
{
    printf("Client -- Received terminate signal.. Server is wating for STOP..\n");
}
// ----------------

// Handle - MSG
void handleMessage(char *ccMsg)
{
    char msg[MAX_MSG_LENGTH];
    unsigned int type;
    sscanf(ccMsg, "%d %s", &type, msg);

    printf("------------------------------------------------\n");
    printf("Chatee says:\t%s\n", msg);
    printf("------------------------------------------------\n");
}
// ----------------

void registerNotification()
{
    struct sigevent ev;
    ev.sigev_notify = SIGEV_SIGNAL;
    ev.sigev_signo = SIGUSR1;

    register_notification(clientQueueDesc, &ev);
}

void handleSignal(int signal)
{

    char *msg = malloc(sizeof(char) * MAX_MSG_LENGTH);
    unsigned int type;

    receive_message(clientQueueDesc, msg, &type);
    if (type == SERVER_CLIENT_CHAT_INIT)
    {
        handleChatInit(msg);
    }
    else if (type == SERVER_CLIENT_TERMINATE)
    {
        handleTerminate();
    }
    else if (type == CLIENT_CLIENT_MSG)
    {
        handleMessage(msg);
    }
    else if (type == CLIENT_CLIENT_DICONNECT)
    {
        handleDisconnect(msg);
    }
    else
    {
        printf("Client -- received message of unknown type..\n");
    }

    free(msg);
    registerNotification();
}

int main(int charc, char *argv[])
{
    srand(time(NULL));
    name = CLIENT_RANDOM_NAME;
    clientQueueDesc = create_queue(name);
    // This is just in case our key was not unique.
    if (clientQueueDesc == -1)
    {
        printf("There already exists client associated with this queue...\n");
        printError();
        return -1;
    }

    serverQueueDesc = get_queue(SERVER_NAME);
    if (serverQueueDesc == -1)
    {
        printf("Failed to open server queue\n");
        printError();
    }

    signal(SIGINT, handleExitSignal);
    signal(SIGUSR1, handleSignal);

    registerMe();
    registerNotification();

    char buffer[MAX_MSG_LENGTH];
    char message[MAX_MSG_LENGTH];

    while (1)
    {
        // Client executes command.
        scanf("%s", buffer);
        if (equals(buffer, "STOP"))
        {
            sendStop();
        }
        else if (equals(buffer, "LIST"))
        {
            sendList();
        }
        else if (equals(buffer, "DISCONNECT"))
        {
            sendDisconnect();
        }
        else if (equals(buffer, "CONNECT"))
        {
            int chateeId;

            scanf("%d", &chateeId);
            sendConnect(chateeId);
        }
        else if (equals(buffer, "SEND"))
        {
            scanf("%s", message);

            if (chateeQueueDesc == -1)
            {
                printf("Client -- unable to send message\n");
            }
            else
            {
                sendMessage(message);
            }
        }
        else if (equals(buffer, "PASS"))
        {
            ;
        }
        else
        {
            printf("Client --  unknown command.\n");
        }
    }

    return 0;
}