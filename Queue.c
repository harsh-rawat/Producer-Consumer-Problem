#include <stdlib.h>
#include <time.h>
#include "Queue.h"
#include "Error.h"

Queue *CreateStringQueue(int size, char* queueIdentity) {

    Queue *stringQueue = malloc(sizeof(Queue));
    if(stringQueue == NULL)
        PrintMallocErrorAndExit(QUEUE_MODULE, queueIdentity, "Queue Structure");

    stringQueue->queueIdentity = queueIdentity;

    stringQueue->capacity = size;
    stringQueue->front = 0;
    stringQueue->end = 0;
    stringQueue->queue = malloc(sizeof(char *) * size);
    if(stringQueue->queue == NULL)
        PrintMallocErrorAndExit(QUEUE_MODULE, queueIdentity, "Queue Array");

    int retVal;
    retVal = sem_init(&stringQueue->lock, 0, 1);
    if(retVal != 0) PrintSemInitErrorAndExit(QUEUE_MODULE, queueIdentity, "Lock");
    retVal = sem_init(&stringQueue->full, 0, 0);
    if(retVal != 0) PrintSemInitErrorAndExit(QUEUE_MODULE, queueIdentity, "Full");
    retVal = sem_init(&stringQueue->empty, 0, size);
    if(retVal != 0) PrintSemInitErrorAndExit(QUEUE_MODULE, queueIdentity, "Empty");

    stringQueue->stats = CreateStatistics(queueIdentity);

    return stringQueue;
}

void EnqueueString(Queue *q, char *string) {

    int retVal;
    clock_t start = clock();
    retVal = sem_wait(&q->empty);
    if(retVal != 0) PrintSemWaitErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Enqueue-Empty");
    retVal = sem_wait(&q->lock);
    if(retVal != 0) PrintSemWaitErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Enqueue-Lock");
    q->queue[q->end] = string;
    q->end = (q->end + 1) % q->capacity;
    UpdateEnqueueCount(q->stats, 1);


    retVal = sem_post(&q->full);
    if(retVal != 0) PrintSemPostErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Enqueue-Full");

    clock_t end = clock();
    UpdateEnqueueTime(q->stats, start, end);

    retVal = sem_post(&q->lock);
    if(retVal != 0) PrintSemPostErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Enqueue-Lock");
}

char *DequeueString(Queue *q) {

    int retVal;
    clock_t start = clock();
    retVal = sem_wait(&q->full);
    if(retVal != 0) PrintSemWaitErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Dequeue-Full");
    retVal = sem_wait(&q->lock);
    if(retVal != 0) PrintSemWaitErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Dequeue-Lock");

    char* string = q->queue[q->front];
    q->front = (q->front + 1) % q->capacity;
    UpdateDequeueCount(q->stats, 1);

    retVal = sem_post(&q->empty);
    if(retVal != 0) PrintSemPostErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Dequeue-Empty");

    clock_t end = clock();
    UpdateDequeueTime(q->stats, start, end);

    retVal = sem_post(&q->lock);
    if(retVal != 0) PrintSemPostErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Dequeue-Lock");

    return string;
}

void PrintQueueStats(Queue *q) {
    PrintStatistics(q->stats);
}
