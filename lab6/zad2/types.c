#include "types.h"
void err(char *mess)
{
    perror(mess);
    exit(0);
}
void close_queue(int queue)
{
    if (mq_close(queue) == -1)
        err("Cant close");
}
void delete_queue(char *name)
{
    if (mq_unlink(name) == -1)
        err("Cant delete");
}
int create_queue(char *name)
{
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MESSAGE_LENGHT - 1;
    attr.mq_curmsgs = 0;

    return mq_open(name, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr);
}
mqd_t get_queue(char *name)
{
    return mq_open(name, O_WRONLY);
}

void send_message(int queue, char mess[MAX_MESSAGE_LENGHT], int type)
{
    if (mq_send(queue, mess, strlen(mess), type) == -1)
        err("Cant send message");
}
void receive_message(int queue, char *mess, int *type)
{
    if (mq_receive(queue, mess, MAX_MESSAGE_LENGHT, type) == -1)
        err("Cant reveive message");
}

char *concat(const char *s1, const char *s2)
{
    int len = strlen(s1) + strlen(s2) + 1;
    char *result = (char *)malloc(len);

    strcpy(result, s1);
    strcat(result, s2);

    return result;
}
char *random_name(int length)
{
    char *str = calloc(length + 1, sizeof(char));

    for (int i = 0; i < length; i++)
    {
        char randomletter = 'a' + (rand() % 26);
        str[i] = randomletter;
    }

    str[length] = '\0';

    return str;
}
