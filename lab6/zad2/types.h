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

#define MAX_MESSAGE_LENGHT 256
#define PREFIX ("/queues-")
#define SERVER_NAME (concat(PREFIX, "server"))
#define CLIENT_RANDOM_NAME (concat(PREFIX, concat("client-", random_name(12))))
typedef struct
{
    int id;
    int queue;
    char *name;
    int available;
} Client;

void err(char *mess);
void close_queue(int queue);
void delete_queue(char *name);
int create_queue(char *name);
mqd_t get_queue(char *name);
void send_message(int queue, char mess[MAX_MESSAGE_LENGHT], int type);
void receive_message(int queue, char *mess, int *type);
char *concat(const char *s1, const char *s2);
char *random_name(int length);
void register_notif(int queue, struct sigevent *ev);
int equals(char *str1, char *str2);