#include "types.h"

int stringEq(char *str1, char *str2) { return strcmp(str1, str2) == 0; }

char *randomString(int length)
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

char *concat(const char *s1, const char *s2)
{
    int len = strlen(s1) + strlen(s2) + 1;
    char *result = (char *)malloc(len);

    strcpy(result, s1);
    strcat(result, s2);

    return result;
}

void printError()
{
    if (errno != 0)
    {
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error printed by perror");
    }
}

void DELETE_QUEUE(char *name) { mq_unlink(name); }
void CLOSE_QUEUE(mqd_t descr) { mq_close(descr); }
int CREATE_QUEUE(char *name)
{
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_LENGTH - 1;
    attr.mq_curmsgs = 0;

    return mq_open(name, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr);
}
int GET_QUEUE(char *name) { return mq_open(name, O_WRONLY); }

void SEND_MESSAGE(mqd_t desc, char *msgPointer, int type) { mq_send(desc, msgPointer, strlen(msgPointer), type); }

void RECEIVE_MESSAGE(mqd_t desc, char *msgPointer, int *typePointer) { mq_receive(desc, msgPointer, MAX_MSG_LENGTH, typePointer); }

void REGISTER_NOTIFICATION(mqd_t desc, struct sigevent *s) { mq_notify(desc, s); }