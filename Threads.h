#ifndef ASSIGNMENT2_THREADS_H
#define ASSIGNMENT2_THREADS_H
#define MAX_BUFFER_SIZE 50960

#include "Queue.h"

#define THREADS_MODULE "Threads"
#define READER "Reader"
#define MUNCH1 "Munch1"
#define MUNCH2 "Munch2"
#define WRITER "Writer"

typedef struct{
    Queue* outputQueue;
} Reader;

typedef struct{
    Queue* inputQueue;
    Queue* outputQueue;
} Munch1;

typedef struct{
    Queue* inputQueue;
    Queue* outputQueue;
} Munch2;

typedef struct{
    Queue* inputQueue;
    int stringsProcessedCount;
} Writer;

Reader* CreateReader(Queue* outputQueue);
Munch1* CreateMunch1(Queue* inputQueue, Queue* outputQueue);
Munch2* CreateMunch2(Queue* inputQueue, Queue* outputQueue);
Writer* CreateWriter(Queue* inputQueue);

void* StartReader(void* ptr);
void* StartMunch1(void* ptr);
void* StartMunch2(void* ptr);
void* StartWriter(void* ptr);

#endif
