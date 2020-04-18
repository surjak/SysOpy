#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "keygen.h"
#include "message.h"
#include "types.h"
#include "que.h"
#include "utils.h"

int server_queue, private_queue;
int friend_queue = -1;
int id;

int sender_pid = 0;

void send_to_server(message_t *message)
{
    if (send(server_queue, message))
    {
        perror("unable to send message to server");
    }
}
void send_to_friend(message_t *message)
{
    printf("%d   PID:  %d\n", friend_queue, getpid());
    if (send(friend_queue, message) == -1)
    {
        perror("unable to send message to friend");
    }
}
void clean()
{
    if (sender_pid != 0)
    {
        message_t message;
        message.type = TYPE_STOP;
        message.id = id;

        send_to_server(&message);

        kill(sender_pid, SIGKILL);
        close_queue(server_queue);
        delete_queue(private_queue, get_private_key());
    }
}

void handle_sigint(int sig)
{
    exit(0);
}

void hanlde_stop()
{
    kill(getppid(), SIGINT);
}

void handle_list()
{
    message_t message;
    message.type = TYPE_LIST;
    message.id = id;
    printf("Client - handle list\n");
    send_to_server(&message);
    printf("Sent to server\n");
}
void handle_connect(int connect_id)
{
    printf("Client %d wants to connect %d\n", id, connect_id);
    message_t message;
    message.type = TYPE_CONNECT;
    message.id = id;
    sprintf(message.text, "%d", connect_id);
    send_to_server(&message);
}
void handle_message_send(char *mess)
{
    printf("%s\n", mess);
    message_t message;
    message.type = TYPE_MESSAGE;
    message.id = id;
    send_to_friend(&message);
    printf("PID: %d", getpid());
}

void sender_handle_line(char *command, char *rest)
{

    if (strcmp("STOP", command) == 0)
    {
        hanlde_stop();
    }
    else if (strcmp("LIST", command) == 0)
    {
        handle_list();
    }
    else if (strcmp("CONNECT", command) == 0)
    {
        handle_connect(atoi(rest));
    }
    else if (strcmp("MESSAGE", command) == 0)
    {
        handle_message_send(rest);
    }
}
int catcher_pid;
void initialize()
{
    if ((server_queue = get_queue(get_public_key())) == -1)
    {
        perror("cant open server queue");
        exit(1);
    }

    key_t private_key = get_private_key();
    if ((private_queue = create_queue(private_key)) == -1)
    {
        perror("cant create private queue");
        exit(1);
    }

    message_t message;
    message.type = TYPE_INIT;
    sprintf(message.text, "%d", private_key);

    if (send(server_queue, &message) == -1)
    {
        perror("unable to register");
        exit(1);
    }

    if (receive(private_queue, &message) == -1)
    {
        perror("unable to register");
        exit(1);
    }

    id = message.id;
    printf("successfully registered with id %d\n", id);
}

void sender()
{
    char line[256];
    char command[256];
    char rest[256];

    while (1)
    {
        fgets(line, 1024, stdin);
        separate_command(line, command, rest);
        sender_handle_line(command, rest);
    }
}

void handle_connect_from_server(message_t *message)
{
    printf("Received CONNECT\n");

    int friend;
    sscanf(message->text, "%d", &friend);
    printf("Friend Queue: %d\n", friend);
    if (friend != -1)
    {
        friend_queue = friend;
        printf("CONENCT with %d\n", friend_queue);
        return;
    }
    printf("CAN'T CONENCT with\n");
}
void handle_message(message_t *message)
{
    printf("Received message \n");
}

void catcher()
{
    message_t message;
    while (1)
    {
        if (receive(private_queue, &message) == -1)
        {
            if (errno != EINTR)
            {
                perror("cant receive message");
                exit(1);
            }
        }
        printf("------\n");
        switch (message.type)
        {
        case TYPE_STOP:
            printf("exiting\n");
            exit(0);
            break;
        case TYPE_CONNECT:
            handle_connect_from_server(&message);
            break;
        case TYPE_MESSAGE:
            handle_message(&message);
            break;
        default:
            break;
        }

        printf("------\n");
    }
}

int main(int argc, char *argv[])
{
    atexit(clean);

    initialize();

    sender_pid = fork();
    if (sender_pid == -1)
    {
        perror("cant fork");
        exit(1);
    }
    else if (sender_pid == 0)
    {
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = SIG_IGN;
        sigaction(SIGINT, &sa, NULL);

        sender();
    }
    else
    {

        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = handle_sigint;
        sigaction(SIGINT, &sa, NULL);

        catcher();
    }

    return 0;
}