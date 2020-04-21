#define TYPES_H
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define CLIENT_SERVER_STOP 5
#define CLIENT_SERVER_DISCONNECT 4
#define CLIENT_SERVER_LIST 3
#define CLIENT_SERVER_CONNECT 2
#define CLIENT_SERVER_INIT 1

#define SERVER_CLIENT_CHAT_INIT 1
#define SERVER_CLIENT_TERMINATE 2
#define SERVER_CLIENT_REGISTRED 3
#define CLIENT_CLIENT_MSG 4
#define CLIENT_CLIENT_DICONNECT 5

#define MAX_CLIENTS 32
#define MAX_MSG_LENGTH 256

#define PREFIX ("/queues-")
#define SERVER_NAME (concat(PREFIX, "server"))
#define CLIENT_RANDOM_NAME \
    (concat(PREFIX, concat("client-", random_string(12))))

void delete_queue(char *name);
void close_queue(mqd_t descr);
int create_queue(char *name);
int get_queue(char *name);
void send_message(mqd_t desc, char *msgPointer, int type);
void receive_message(mqd_t desc, char *msgPointer, int *typePointer);
void register_notification(mqd_t desc, struct sigevent *s);
void printError();
int equals(char *str1, char *str2);
char *random_string(int length);
char *concat(const char *s1, const char *s2);

struct Client
{
    int clientId;
    int queueDesc;
    char *name;
    int available;
} typedef Client;