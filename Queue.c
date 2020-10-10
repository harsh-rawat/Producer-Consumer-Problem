#include "Queue.h"
#include <stdlib.h>
#include <time.h>

Queue *CreateStringQueue(int size, char* queueIdentity) {

    Queue *stringQueue = malloc(sizeof(Queue));
    stringQueue->queueIdentity = queueIdentity;

    stringQueue->capacity = size;
    stringQueue->front = 0;
    stringQueue->end = 0;
    stringQueue->queue = malloc(sizeof(char *) * size);

    sem_init(&stringQueue->lock, 0, 1);
    sem_init(&stringQueue->full, 0, 0);
    sem_init(&stringQueue->empty, 0, size);

    stringQueue->stats = CreateStatistics();

    return stringQueue;
}

void EnqueueString(Queue *q, char *string) {

    clock_t start = clock();
    sem_wait(&q->empty);
    sem_wait(&q->lock);
    q->queue[q->end] = string;
    q->end = (q->end + 1) % q->capacity;
    UpdateEnqueueCount(q->stats, 1);


    sem_post(&q->full);

    clock_t end = clock();
    UpdateEnqueueTime(q->stats, start, end);

    sem_post(&q->lock);
}

char *DequeueString(Queue *q) {

    clock_t start = clock();
    sem_wait(&q->full);
    sem_wait(&q->lock);

    char* string = q->queue[q->front];
    q->front = (q->front + 1) % q->capacity;
    UpdateDequeueCount(q->stats, 1);

    sem_post(&q->empty);

    clock_t end = clock();
    UpdateDequeueTime(q->stats, start, end);

    sem_post(&q->lock);
    return string;
}

void PrintQueueStats(Queue *q) {
    PrintStatistics(q->stats, q->queueIdentity);
}
