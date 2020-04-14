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
#include "queue.h"
int server_queue, private_queue;
int id;

int sender_pid = 0;

void send_to_server(message *message)
{
    if (send(server_queue, message))
    {
        perror("unable to send message to server");
    }
}
void clean()
{
    if (sender_pid != 0)
    {
        message message;
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
void initialize()
{
    if ((server_queue = get_queue(get_public_key())) == -1)
    {
        perror("can't open server queue");
        exit(1);
    }
    key_t private_key = get_private_key();
    if ((private_queue = create_queue(private_key)) == -1)
    {
        perror("cant create private queue");
        exit(1);
    }

    message mess;
    mess.type = TYPE_INIT;
    sprintf(mess.text, "%d", private_key);
    if (send(server_queue, &mess) == -1)
    {
        perror("unable to register");
        exit(1);
    }
    if (receive(private_queue, &mess) == -1)
    {
        perror("unable to register");
        exit(1);
    }

    id = mess.id;
    printf("successfully registered with id %d\n", id);
}
int main()
{
    atexit(clean);
    initialize();
}