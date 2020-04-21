#include "types.h"
#include <signal.h>

int client_queue;
int server_queue;
int id;
char *name;

// When chatting
int chateequeue = -1;

void exitClient()
{
    printf("Client -- exit..\n");
    close_queue(server_queue);
    close_queue(client_queue);

    delete_queue(name);
    exit(EXIT_SUCCESS);
}

// SEND - INIT
void registerMe()
{
    char csMsg[MAX_MESSAGE_SIZE];
    sprintf(csMsg, "%d %s", CLIENT_SERVER_INIT, name);

    send_message(server_queue, csMsg, CLIENT_SERVER_INIT);

    char scMsg[MAX_MESSAGE_SIZE];
    unsigned int type;
    receive_message(client_queue, scMsg, &type);

    sscanf(scMsg, "%d %d", &type, &id);
    printf("Client -- registered with id: %d, key: %s\n", id, name);
}
// ----------------

// SEND - STOP
void sendStop()
{
    printf("Client -- sending STOP..\n");

    char csMsg[MAX_MESSAGE_SIZE];
    sprintf(csMsg, "%d %d", CLIENT_SERVER_STOP, id);
    send_message(server_queue, csMsg, CLIENT_SERVER_STOP);

    exitClient();
}

void handleExitSignal(int sig) { sendStop(); }
// ----------------

// SEND - LIST
void sendList()
{
    printf("Client -- sending LIST..\n");

    char csMsg[MAX_MESSAGE_SIZE];
    sprintf(csMsg, "%d %d", CLIENT_SERVER_LIST, id);
    send_message(server_queue, csMsg, CLIENT_SERVER_LIST);
}
// ----------------

// SEND - DISCONNECT
void sendDisconnect()
{
    printf("Client -- disconnected chat\n");

    char csMsg[MAX_MESSAGE_SIZE];
    sprintf(csMsg, "%d %d", CLIENT_SERVER_DISCONNECT, id);
    send_message(server_queue, csMsg, CLIENT_SERVER_DISCONNECT);

    if (chateequeue != -1)
    {
        char ccMsg[MAX_MESSAGE_SIZE];
        sprintf(csMsg, "%d %d", CLIENT_CLIENT_DICONNECT, id);
        send_message(chateequeue, ccMsg, CLIENT_CLIENT_DICONNECT);

        close_queue(chateequeue);
        chateequeue = -1;
    }
}
// ----------------

// SEND - CONNECT
void sendConnect(int chateeId)
{
    printf("Client -- seending CONNECT to chatee with ID: %d\n", chateeId);

    char csMsg[MAX_MESSAGE_SIZE];
    sprintf(csMsg, "%d %d %d", CLIENT_SERVER_CONNECT, id, chateeId);
    send_message(server_queue, csMsg, CLIENT_SERVER_CONNECT);
}
// ----------------

// SEND - MSG
void sendMessage(char *message)
{
    char ccMsg[MAX_MESSAGE_SIZE];
    sprintf(ccMsg, "%d %s", CLIENT_CLIENT_MSG, message);

    send_message(chateequeue, ccMsg, CLIENT_CLIENT_MSG);
    printf("------------------------------------------------\n");
    printf("Me:          \t%s\n", message);
    printf("------------------------------------------------\n");
}
// ----------------

// HANDLE - DISCONNECT
void handle_disconnect(char *msg)
{
    printf("Client -- received disconnect msg from chatee..\n");

    chateequeue = -1;
    close_queue(chateequeue);
    sendDisconnect();
}
// ----------------

// Handle - CHAT_INIT
void handleChatInit(char *msg)
{
    int chateeId;
    unsigned int type;
    char chateeName[MAX_MESSAGE_SIZE];
    sscanf(msg, "%d %d %s", &type, &chateeId, chateeName);
    printf("Client -- entering chat with %d..\n", chateeId);
    chateequeue = get_queue(chateeName);
}
// ----------------

// Handle - TERMINATE
void handleTerminate()
{
    printf("Client -- Received terminate signal.. Server is wating for STOP..\n");
}
// ----------------

// Handle - MSG
void handle_message(char *ccMsg)
{
    char msg[MAX_MESSAGE_SIZE];
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

    register_notification(client_queue, &ev);
}

void handleSignal(int signal)
{

    char *msg = malloc(sizeof(char) * MAX_MESSAGE_SIZE);
    unsigned int type;

    receive_message(client_queue, msg, &type);
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
        handle_message(msg);
    }
    else if (type == CLIENT_CLIENT_DICONNECT)
    {
        handle_disconnect(msg);
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
    client_queue = create_queue(name);
    // This is just in case our key was not unique.
    if (client_queue == -1)
    {
        printf("There already exists client associated with this queue...\n");
        printError();
        return -1;
    }

    server_queue = get_queue(SERVER_NAME);
    if (server_queue == -1)
    {
        printf("Failed to open server queue\n");
        printError();
    }

    signal(SIGINT, handleExitSignal);
    signal(SIGUSR1, handleSignal);

    registerMe();
    registerNotification();

    char buffer[MAX_MESSAGE_SIZE];
    char message[MAX_MESSAGE_SIZE];

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

            if (chateequeue == -1)
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