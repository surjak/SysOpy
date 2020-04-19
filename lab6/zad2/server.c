#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <mqueue.h>

#include "types.h"
#define MAX_CLIENTS 128
char *clients[MAX_CLIENTS][2];

mqd_t server_queue;
void on_exit(int sig)
{
    char *message = calloc(MAX_MESSAGE_LENGHT, sizeof(char));
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i][0] != NULL)
        {
            mqd_t client_queue = mq_open(clients[i][0], O_RDWR);
            if (client_queue == -1)
            {
                perror("Can't access client queue\n");
                exit(0);
            }
            if (mq_send(client_queue, message, MAX_MESSAGE_LENGHT, TYPE_STOP) < 0)
            {
                perror("Can't send message to client\n");
                exit(0);
            }
            if (mq_receive(server_queue, message, MAX_MESSAGE_LENGHT, NULL) < 0)
            {
                perror("Can't receive message from client\n");
                exit(0);
            }
            if (mq_close(client_queue) < 0)
            {
                perror("Can't close client queue\n");
                exit(0);
            }
        }
    }

    if (mq_close(server_queue) < 0)
    {
        perror("Can't close server queue\n");
        exit(0);
    }
    if (mq_unlink(SERVER_NAME) < 0)
    {
        perror("Can't delete server queue\n");
        exit(0);
    }
    exit(0);
}
int find_cliend_id()
{

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i][0] == NULL)
        {
            return i;
        }
    }

    return -1;
}
void open_server_queue()
{
    server_queue = mq_open(SERVER_NAME, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    if (server_queue == -1)
    {
        perror("Can't open server queue");
        exit(0);
    }
}
void init()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i][0] = NULL;
        clients[i][1] = NULL;
    }
}

int main()
{
    init();
    open_server_queue();
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = on_exit;
    sigaction(SIGINT, &sa, NULL);
}