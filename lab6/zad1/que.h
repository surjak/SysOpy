#ifndef QUE_H
#define QUE_H
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "message.h"

int send(int queue, message_t *message);

int receive(int queue, message_t *message);

int create_queue(int key);

int delete_queue(int queue, int key);

int get_queue(int key);

int close_queue(int queue);

int isQueueEmpty(int queueId);
int receive_no_wait(int queue, message_t *message);

#define SOMETHING_HAPPEND SIGRTMIN

#endif