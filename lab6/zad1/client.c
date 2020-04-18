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

int friend_pid = 0;

void send_to_server(message_t *message)
{
    if (send(server_queue, message))
    {
        perror("unable to send message to server");
    }
}
void send_to_friend(message_t *message)
{
    if (send(friend_queue, message) == -1)
    {
        perror("unable to send message to friend");
    }
}
void clean()
{
    message_t message;
    message.type = TYPE_STOP;
    message.id = id;

    send_to_server(&message);
    close_queue(server_queue);
    delete_queue(private_queue, get_private_key());
}

void handle_sigint(int sig)
{
    exit(0);
}

void hanlde_stop()
{
    kill(getpid(), SIGINT);
}

void handle_list()
{
    message_t message;
    message.type = TYPE_LIST;
    message.id = id;
    send_to_server(&message);
}

void handle_disconnect()
{

    message_t message;
    message.type = TYPE_DISCONNECT;
    message.id = id;
    send_to_friend(&message);
    friend_queue = -1;
    printf("DISCONNECTED");
    send_to_server(&message);
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
    message_t message;
    message.type = TYPE_MESSAGE;
    message.id = id;
    sprintf(message.text, "%s", mess);
    send_to_friend(&message);
    printf("_________\n");
    printf("M: %s\n", mess);
    printf("_________\n");
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
    else if (strcmp("DISCONNECT", command) == 0)
    {
        handle_disconnect();
    }
}

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
    message.pid = getpid();

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

    fgets(line, 1024, stdin);
    separate_command(line, command, rest);
    sender_handle_line(command, rest);
}

void handle_connect_from_server(message_t *message)
{

    int friend;
    sscanf(message->text, "%d", &friend);
    if (friend != -1)
    {
        friend_queue = friend;
        printf("CONENCT with %d\n", friend_queue);
        friend_pid = message->pid;
        printf("_____WELCOME IN CHAT_____\n");
        return;
    }
    printf("CAN'T CONENCT with\n");
}
void handle_message(message_t *message)
{
    printf("_________\n");
    char mess[256];
    sprintf(mess, "%s", message->text);
    printf("F: %s\n", mess);
    printf("_________\n");
}

void handle_disconnect_from_server(message_t *message)
{
    friend_queue = -1;
    printf("DISCONNECTED");
}

void catcher()
{

    signal(SIGINT, handle_sigint);

    while (!is_empty(private_queue))
    {
        message_t message;

        if (receive_no_wait(private_queue, &message) != -1)
        {

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
            case TYPE_DISCONNECT:
                handle_disconnect_from_server(&message);
                break;
            default:
                break;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    atexit(clean);
    signal(SOMETHING_HAPPEND, catcher);
    signal(SIGINT, handle_sigint);
    initialize();
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = catcher;
    sigaction(SOMETHING_HAPPEND, &sa, NULL);
    while (1)
    {

        sender();
        catcher();
    }

    return 0;
}