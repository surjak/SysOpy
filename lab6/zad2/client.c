#define _POSIX_C_SOURCE 199309L
#include "types.h"
#include <signal.h>

int client_queue;
int server_queue;
int id;
char *name;

int friend_queue;
void handle_exit()
{
    printf("Client exits\n");
    close_queue(server_queue);
    close_queue(client_queue);
    delete_queue(name);
    exit(EXIT_SUCCESS);
}

void register_notifications()
{
    struct sigevent ev;
    ev.sigev_notify = SIGEV_SIGNAL;
    ev.sigev_signo = SIGUSR1;

    register_notif(client_queue, &ev);
}
void handle_signal(int signal)
{
    //todo
    char *msg = malloc(sizeof(char) * MAX_MESSAGE_LENGHT);
    unsigned int type;
    receive_message(client_queue, msg, &type);
    switch (type)
    {
    case SERVER_CLIENT_CHAT_INIT:
        break;
    case SERVER_CLIENT_TERMINATE:
        break;
    case CLIENT_CLIENT_MSG:
        break;
    case CLIENT_CLIENT_DICONNECT:
        break;
    default:
        printf("Client -- received message of unknown type..\n");
    }
    free(msg);
    register_notifications();
}

void register_me()
{
    char mess[MAX_MESSAGE_LENGHT];
    sprintf(mess, "%d %s", CLIENT_SERVER_INIT, name);
    send_message(server_queue, mess, CLIENT_SERVER_INIT);
    char back[MAX_MESSAGE_LENGHT];
    unsigned int type;
    receive_message(client_queue, back, &type);
    sscanf(back, "%d %d", &type, &id);
    printf("Client -- registered with id: %d, key: %s\n", id, name);
}

void send_list()
{
    printf("Client -- LIST..\n");
    char mess[MAX_MESSAGE_LENGHT];
    sprintf(mess, "%d %d", CLIENT_SERVER_LIST, id);
    send_message(server_queue, mess, CLIENT_SERVER_LIST);
}

void send_stop()
{
    printf("Client -- STOP..\n");

    char mess[MAX_MESSAGE_LENGHT];
    sprintf(mess, "%d %d", CLIENT_SERVER_STOP, id);
    send_message(server_queue, mess, CLIENT_SERVER_STOP);

    handle_exit();
}
void stop(int sig) { send_stop(); }
int main()
{
    srand(time(NULL));
    name = CLIENT_RANDOM_NAME;
    client_queue = create_queue(name);
    if (client_queue == -1)
    {
        err("Queue already exists");
        return -1;
    }
    server_queue = get_queue(SERVER_NAME);
    if (server_queue == -1)
    {
        err("Cant open server queue");
        return -1;
    }
    signal(SIGINT, stop);
    signal(SIGUSR1, handle_signal);
    register_me();
    register_notifications();
    char buffer[MAX_MESSAGE_LENGHT];
    char message[MAX_MESSAGE_LENGHT];
    while (1)
    {
        scanf("%s", buffer);
        if (equals(buffer, "STOP"))
        {
            send_stop();
        }
        else if (equals(buffer, "LIST"))
        {
            send_list();
        }
        else if (equals(buffer, "DISCONNECT"))
        {
            //
        }
        else if (equals(buffer, "CONNECT"))
        { //
        }
        else if (equals(buffer, "MESSAGE"))
        {
            //
        }
        else if (equals(buffer, "PASS"))
        {
            //
        }
        else
        {
            printf("Client --  unknown command.\n");
        }
    }

    return 0;
}