#include "shared.h"

int get_semaphore()
{
    key_t sem_key = ftok(getenv("HOME"), 0);
    int semaphore_id = semget(sem_key, 0, 0);
    if (semaphore_id < 0)
    {
        printf("Can't get semaphore %d\n", errno);
        exit(EXIT_FAILURE);
    }
    return semaphore_id;
}

int get_shared_memory()
{
    key_t shm_key = ftok(getenv("HOME"), 1);
    int shared_memory_id = shmget(shm_key, 0, 0);
    if (shared_memory_id < 0)
    {
        printf("Can't access shared memory %d\n", errno);
        exit(EXIT_FAILURE);
    }
    return shared_memory_id;
}
