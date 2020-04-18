#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "message.h"
#include "types.h"

int send(int queue, message_t *message)
{
    return msgsnd(queue, message, MAX_MESSAGE_SIZE, 0);
}

int receive(int queue, message_t *message)
{
    return msgrcv(queue, message, MAX_MESSAGE_SIZE, -TYPE_INIT, 0);
}

int create_queue(int key)
{
    return msgget(key, IPC_CREAT | IPC_EXCL | 0600);
}

int delete_queue(int queue, int key)
{
    return msgctl(queue, IPC_RMID, NULL);
}

int get_queue(int key)
{
    return msgget(key, 0);
}

int close_queue(int queue)
{
    return 0;
}