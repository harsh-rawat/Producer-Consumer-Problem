/**
 * @author Harsh Rawat, harsh-rawat, hrawat2
 * @author Sidharth Gurbani, gurbani, gurbani
 * */

#include <stdlib.h>
#include <time.h>
#include "Queue.h"
#include "Error.h"

/**
 * @function CreateStringQueue
 * @argument size - size of the queue
 * @argument queueIdentity - Name associated with the queue
 * @description This method initializes a new instance of the queue and returns the same.
 * In case of an error, an appropriate message is printed on the stderr and then method exits using failure code.
 * */
Queue *CreateStringQueue(int size, char* queueIdentity) {

    // Allocate the space for Queue struct using malloc
    Queue *stringQueue = malloc(sizeof(Queue));
    // If malloc returns an error then print the error message and exit
    if(stringQueue == NULL) {
        PrintMallocErrorAndExit(QUEUE_MODULE, queueIdentity, "Queue Structure");
        return NULL;
    }

    // Set the name associated with the queue
    stringQueue->queueIdentity = queueIdentity;

    // Set the size of the queue
    stringQueue->capacity = size;
    // Initialise the front and end of the queue to 0
    stringQueue->front = 0;
    stringQueue->end = 0;
    // Allocate space for the string array
    stringQueue->queue = malloc(sizeof(char *) * size);
    // If malloc returns an error then print the corresponding error message and exit
    if(stringQueue->queue == NULL) {
        free(stringQueue);
        PrintMallocErrorAndExit(QUEUE_MODULE, queueIdentity, "Queue Array");
        return NULL;
    }

    int retVal;
    // Initialize lock semaphore with initial value of 1. In case of error print error message and exit.
    retVal = sem_init(&stringQueue->lock, 0, 1);
    if(retVal != 0) PrintSemInitErrorAndExit(QUEUE_MODULE, queueIdentity, "Lock");
    // Initialize full semaphore with initial value of 0. In case of error print error message and exit.
    retVal = sem_init(&stringQueue->full, 0, 0);
    if(retVal != 0) PrintSemInitErrorAndExit(QUEUE_MODULE, queueIdentity, "Full");
    // Initialize empty semaphore with initial value of size. In case of error print error message and exit.
    // This is done as initially the queue would be empty and would have 'size' available slots.
    retVal = sem_init(&stringQueue->empty, 0, size);
    if(retVal != 0) PrintSemInitErrorAndExit(QUEUE_MODULE, queueIdentity, "Empty");

    // Create stats struct by calling the appropriate method from statistics module
    stringQueue->stats = CreateStatistics(queueIdentity);

    return stringQueue;
}

/**
 * @function EnqueueString
 * @argument q - Queue struct
 * @argument string - String to be enqueued
 * @description
 * Enqueue the given string in the given queue.
 * The access to this queue should be synchronized.
 * */
void EnqueueString(Queue *q, char *string) {

    int retVal;
    // Start the clock timer
    clock_t start = clock();
    // Check if the queue has empty slot. If so the proceed else wait.
    retVal = sem_wait(&q->empty);
    // In case of error print error message and exit
    if(retVal != 0) PrintSemWaitErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Enqueue-Empty");
    // If we have an empty slot then lock this function or wait to lock it.
    retVal = sem_wait(&q->lock);
    // In case of error print error message and exit
    if(retVal != 0) PrintSemWaitErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Enqueue-Lock");

    // Enqueue the string and update the enqueue count
    q->queue[q->end] = string;
    q->end = (q->end + 1) % q->capacity;
    UpdateEnqueueCount(q->stats, 1);


    // Increment the full semaphore to indicate that an entry was added
    retVal = sem_post(&q->full);
    if(retVal != 0) PrintSemPostErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Enqueue-Full");

    // End clock timer and update the enqueue time.
    clock_t end = clock();
    UpdateEnqueueTime(q->stats, start, end);

    // Release the lock on this method
    retVal = sem_post(&q->lock);
    if(retVal != 0) PrintSemPostErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Enqueue-Lock");
}

/**
 * @function DequeueString
 * @argument q - Queue struct
 * @description
 * Dequeue a string from the queue.
 * The access to this queue should be synchronized.
 * */
char *DequeueString(Queue *q) {

    int retVal;
    // Start the clock
    clock_t start = clock();
    // Check if the queue has some entry which can be dequeued. If so then proceed else wait for an entry to be added.
    retVal = sem_wait(&q->full);
    // In case of error print error message and exit
    if(retVal != 0) PrintSemWaitErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Dequeue-Full");
    // If the entry is available in queue for dequeue then lock this method or wait to lock it.
    retVal = sem_wait(&q->lock);
    // In case of error print error message and exit
    if(retVal != 0) PrintSemWaitErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Dequeue-Lock");

    // Dequeue a string from the queue.
    char* string = q->queue[q->front];
    q->front = (q->front + 1) % q->capacity;
    UpdateDequeueCount(q->stats, 1);

    // Update the semaphore to indicate that an empty slot is available due to dequeue
    retVal = sem_post(&q->empty);
    if(retVal != 0) PrintSemPostErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Dequeue-Empty");

    // end the clock and update the dequeue time
    clock_t end = clock();
    UpdateDequeueTime(q->stats, start, end);

    // Release the lock on this method.
    retVal = sem_post(&q->lock);
    if(retVal != 0) PrintSemPostErrorAndExit(QUEUE_MODULE, q->queueIdentity, "Dequeue-Lock");

    // return the dequeued string
    return string;
}

/**
 * @function PrintQueueStats
 * @argument q - queue struct
 * @description Print the stats of the queue
 * */
void PrintQueueStats(Queue *q) {
    // Call the method from statistics module to print the queue stats
    PrintStatistics(q->stats);
}
