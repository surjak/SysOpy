#define _XOPEN_SOURCE 500

#include "keygen.h"

#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdio.h>

key_t get_public_key()
{
    key_t key;
    if ((key = ftok(getenv("HOME"), 1)) == -1)
    {
        perror("Can't generate public key");
    }
    return key;
}

key_t get_private_key()
{
    key_t key;
    if ((key = ftok(getenv("HOME"), getpid())) == -1)
    {
        perror("Can't generate private key");
    }
    return key;
}