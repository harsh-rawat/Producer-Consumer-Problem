/**
 * @author Harsh Rawat, harsh-rawat, hrawat2
 * @author Sidharth Gurbani, gurbani, gurbani
 *
 * @description
 * This module implements the functionality of a synchronized queue.
 * Each enqueue and dequeue operation is locked before any operation is performed.
 * We use semaphores available in semaphore.h for synchronization.
 * Actual queue is implemented as an array of input size.
 * The statistics of the queue are recorded using Statistics module
 *
 * @functions
 * CreateStringQueue - Return an initialized Queue struct which can be used directly.
 * EnqueueString - Enqueue a string in the queue
 * DequeueString - Dequeue a string from the queue
 * PrintQueueStats - Print the stats of the queue
 *
 * */

#ifndef ASSIGNMENT2_QUEUE_H
#define ASSIGNMENT2_QUEUE_H

#include <semaphore.h>
#include "statistics.h"

#define QUEUE_MODULE "Queue"

// The struct of queue which stores all the variables used for implementing the queue functionality
typedef struct {
    // Name of the queue
    char* queueIdentity;

    // Capacity is the size of the queue
    int capacity;
    // front stores the front index at which an element is present
    int front;
    // end stores the end index at which an element would be inserted
    int end;
    // Array of strings which store the actual data
    char** queue;

    // Semaphore for locking the method before performing any operation
    sem_t lock;
    // Semaphore to indicate that the queue is full
    sem_t full;
    // Semaphore to indicate that the queue is empty
    sem_t empty;

    // A struct of stats module which stores the statistics of this queue
    Stats* stats;
} Queue;

Queue *CreateStringQueue(int size, char* queueIdentity);
void EnqueueString(Queue *q, char *string);
char * DequeueString(Queue *q);
void PrintQueueStats(Queue *q);

#endif
