#ifndef MESSAGE_H
#define MESSAGE_H

#define MESSAGE_BUFFER_SIZE 1024

typedef struct message_t
{
    long type;
    time_t timestamp;
    int id;
    char text[MESSAGE_BUFFER_SIZE];
} message_t;

#define MAX_MESSAGE_SIZE sizeof(message_t) - sizeof(long)
#define ALL_MESSAGE_SIZE sizeof(message_t)
#define MAX_MESSAGES 10

#endif