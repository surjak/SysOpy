#include "types.h"
#include <signal.h>

int client_queue;
int server_queue;
int id;
char *name;

int friend_queue;
void handle_exit()
{
    printf("Client exits\n");
    close_queue(server_queue);
    close_queue(client_queue);
    delete_queue(name);
    exit(EXIT_SUCCESS);
}

int main()
{
    srand(time(NULL));
    name = CLIENT_RANDOM_NAME;
    client_queue = create_queue(name);
    if (client_queue == -1)
    {
        err("Queue already exists");
        return -1;
    }
    server_queue = get_queue(SERVER_NAME);
    if (server_queue == -1)
    {
        err("Cant open server queue");
        return -1;
    }
    signal(SIGINT, handle_exit);
    return 0;
}