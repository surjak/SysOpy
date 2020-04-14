#ifndef MESSAGE_H
#define MESSAGE_H
#include <time.h>
#define MESSAGE_SIZE 1024

typedef struct
{
    long type;
    time_t timestamp;
    int id;
    char text[MESSAGE_SIZE];
} message;

#define MAX_MESSAGE_SIZE sizeof(message) - sizeof(long)
#define All_MESSAGE_SIZE sizeof(message)
#define MAX_MESSAGES 10

#endif