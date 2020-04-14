#ifndef QUE_H
#define QUE_H

#include "message.h"

int send(int queue, message *message);

int receive(int queue, message *message);

int create_queue(int key);

int delete_queue(int queue, int key);

int get_queue(int key);

int close_queue(int queue);

#endif