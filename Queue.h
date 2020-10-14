#ifndef ASSIGNMENT2_QUEUE_H
#define ASSIGNMENT2_QUEUE_H

#include <semaphore.h>
#include "statistics.h"

#define QUEUE_MODULE "Queue"

typedef struct {
    char* queueIdentity;

    int capacity;
    int front;
    int end;
    char** queue;

    sem_t lock;
    sem_t full;
    sem_t empty;

    Stats* stats;
} Queue;

Queue *CreateStringQueue(int size, char* queueIdentity);
void EnqueueString(Queue *q, char *string);
char * DequeueString(Queue *q);
void PrintQueueStats(Queue *q);

#endif
