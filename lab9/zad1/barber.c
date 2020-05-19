#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/times.h>
#include <time.h>

int count_of_clients;
int count_of_chairs;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

pthread_t current_client;

int next_chair = 0;
int next_chair_for_barber = 0;
int clients_served = 0;

pthread_t *chairs;
int sleeping = 0;
int free_chairs;
void *barber()
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        if (free_chairs == count_of_chairs)
        {
            printf("Golibroda: ide spac\n");
            sleeping = 1;
            pthread_cond_wait(&condition, &mutex);
            sleeping = 0;
        }
        else
        {
            free_chairs++;
            current_client = chairs[next_chair_for_barber];
            next_chair_for_barber = (next_chair_for_barber + 1) % count_of_chairs;
        }
        printf("Golibroda: czeka %d klientow, gole klienta %ld\n", count_of_chairs - free_chairs, current_client);
        pthread_mutex_unlock(&mutex);
        sleep((rand() % 3 + 1));
        pthread_mutex_lock(&mutex);
        clients_served++;
        if (clients_served == count_of_clients)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit((void *)0);
}

void *client()
{
    pthread_t client_id = pthread_self();
    while (1)
    {
        pthread_mutex_lock(&mutex);
        if (sleeping)
        {
            current_client = client_id;
            printf("Klient: budze golibrode; %ld\n", client_id);
            pthread_cond_broadcast(&condition);
            break;
        }
        else if (free_chairs > 0)
        {
            chairs[next_chair] = client_id;
            next_chair = (next_chair + 1) % count_of_chairs;
            free_chairs--;
            printf("Klient: poczekalnia, wolne miejsca %d; %ld\n", free_chairs, client_id);
            break;
        }
        else
        {
            printf("Klient: zajete; %ld\n", client_id);
            pthread_mutex_unlock(&mutex);
            sleep((rand() % 3 + 1) * 4);
        }
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit((void *)0);
}
void run_threads()
{
    pthread_t *clients = calloc(count_of_clients + 1, sizeof(pthread_t));

    pthread_create(&clients[0], NULL, barber, NULL);
    for (int i = 1; i < count_of_clients + 1; i++)
    {
        sleep(rand() % 3 + 1);
        pthread_create(&clients[i], NULL, client, NULL);
    }

    for (int i = 0; i < count_of_clients + 1; i++)
    {
        pthread_join(clients[i], NULL);
    }
}

int main(int args, char **argv)
{
    count_of_chairs = atoi(argv[1]);
    count_of_clients = atoi(argv[2]);
    free_chairs = count_of_chairs;
    srand(time(NULL));
    chairs = calloc(count_of_chairs, sizeof(pthread_t));
    run_threads();
    return 0;
}