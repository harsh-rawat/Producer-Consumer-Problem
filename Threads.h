/**
 * @author Harsh Rawat (hrawat2)
 * @author Sidharth Gurbani (gurbani)
 *
 * @description
 * This module implements the main functionality of Consumer-Producer problem.
 * We have a reader which reads the data from stdin and writes the same to its queue.
 * Munch1 runs in its own thread and waits for Reader to enqueue a string on the queue.
 * It takes the string and replaces spaces with *
 * Munch2 runs in its own thread and waits for Munch1 to complete its task and enqueue the string in the shared queue.
 * It takes the string and converts lower case to upper case.
 * Writer is the last thread which takes the string from shared queue and writes it to stdout
 * In case of any error in any thread, we print an error message and exit with failure code.
 *
 * @functions
 * CreateReader - Create a reader struct
 * CreateMunch1 - Create a Munch1 struct
 * CreateMunch2 - Create a Munch2 struct
 * CreateWriter - Create a Writer struct
 *
 * All the methods below run in their own thread.
 * StartReader - Read from stdin as per given constraints and enqueue the string in shared queue with Munch1
 * StartMunch1 - Take the string from shared queue with reader and perform Munch1 operation.
 * StartMunch2 - Take the string from shared queue with Munch1 and perform Munch2 operation.
 * StartWriter - Take the string from shared queue with Munch2 and write the same to stdout
 * */

#ifndef ASSIGNMENT2_THREADS_H
#include "Queue.h"


#define ASSIGNMENT2_THREADS_H
// Define maximum buffer size to be used for any string
#define MAX_BUFFER_SIZE 50960
// Define constants for various strings
#define THREADS_MODULE "Threads"
#define READER "Reader"
#define MUNCH1 "Munch1"
#define MUNCH2 "Munch2"
#define WRITER "Writer"

// Struct for Reader
typedef struct{
    // Shared queue of Reader-Munch1
    Queue* outputQueue;
} Reader;

// Struct for Munch1
typedef struct{
    // Shared queue of Reader-Munch1
    Queue* inputQueue;
    // Shared queue of Munch1-Munch2
    Queue* outputQueue;
} Munch1;

// Struct for Munch2
typedef struct{
    // Shared queue of Munch1-Munch2
    Queue* inputQueue;
    // Shared queue of Munch2-Writer
    Queue* outputQueue;
} Munch2;

// Struct for Writer
typedef struct{
    // Shared queue of Munch2-Writer
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
