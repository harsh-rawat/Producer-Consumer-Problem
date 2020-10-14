/**
 * @author Harsh Rawat (hrawat2)
 * @author Sidharth Gurbani (gurbani)
 *
 * @description This is the main method which creates all the threads after initializing appropriate data structures.
 *
 * @functions
 * main - main method
 * findErrorIndex - Given an array containing return codes, returns the first non-zero code which would signify error.
 *
 * */

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "Queue.h"
#include "Threads.h"
#include "Error.h"

// The maximum size of each queue
#define MAX_QUEUE_SIZE 10

// static function to find the index of error code in an array.
static int findErrorIndex(int* retVals);

/**
 * @function main
 * @arguments None
 * @description
 * This method creates 3 queues and then calls functions from Thread module to create Reader, Munch1, Munch2 and Writer structs.
 * Subsequently, it creates 4 threads corresponding to each function and waits for them to finish using join.
 * Before exiting, it prints the stats for each queue.
 * In case of any error, an appropriate message is printed on stderr and then the program exits.
 * */
int main(){
    pthread_t reader_thread, munch1_thread, munch2_thread, writer_thread;

    // Create a queue to act as an intermediary between 4 functionalities i.e. Reader, Munch1, Munch2 and Writer.
    Queue* reader_munch1_queue = CreateStringQueue(MAX_QUEUE_SIZE, "Reader-Munch1");
    Queue* munch1_munch2_queue = CreateStringQueue(MAX_QUEUE_SIZE, "Munch1-Munch2");
    Queue* munch2_writer_queue = CreateStringQueue(MAX_QUEUE_SIZE, "Munch2-Writer");

    // Call the methods from Thread module to create the appropriate structs for each function.
    // The queue created above are passed to each struct.
    Reader* reader = CreateReader(reader_munch1_queue);
    Munch1* munch1 = CreateMunch1(reader_munch1_queue, munch1_munch2_queue);
    Munch2* munch2 = CreateMunch2(munch1_munch2_queue, munch2_writer_queue);
    Writer* writer = CreateWriter(munch2_writer_queue);

    // Create the threads using the functional structs created above. We store the return value in an array.
    int thread_rets[4];
    thread_rets[0] = pthread_create(&reader_thread, NULL, StartReader, (void*) reader);
    thread_rets[1] = pthread_create(&munch1_thread, NULL, StartMunch1, (void*) munch1);
    thread_rets[2] = pthread_create(&munch2_thread, NULL, StartMunch2, (void*) munch2);
    thread_rets[3] = pthread_create(&writer_thread, NULL, StartWriter, (void*) writer);

    // From the return value array, find the index of error, if any.
    int errorIndex = findErrorIndex(thread_rets);
    if(errorIndex != -1){
        // In case of an error, print the corresponding message and exit.
        PrintErrorAndExit(errorIndex+1, thread_rets[errorIndex]);
    }

    // Wait for the threads to finish execution
    pthread_join(reader_thread, NULL);
    pthread_join(munch1_thread, NULL);
    pthread_join(munch2_thread, NULL);
    pthread_join(writer_thread, NULL);

    // Once the execution is completed by the threads, we print the stats of each queue.
    PrintQueueStats(reader_munch1_queue);
    PrintQueueStats(munch1_munch2_queue);
    PrintQueueStats(munch2_writer_queue);

    // exit with a success response
    exit(EXIT_SUCCESS);
}

/**
 * @function findErrorIndex
 * @arguments retVals - Pointer to an integer array
 * @description
 * This function takes an array pointer as an input and then return the index of first non-zero value in the array.
 * The first non-zero value would correspond to the error.
 * */
static int findErrorIndex(int* retVals){
    int errorIndex = -1;
    // Iterate through the array and check if the corresponding value is non-zero
    for(int index = 0; index < 4; index++){
        if(retVals[index]){
            // If a non-zero value is found, break the loop and return the value.
            errorIndex = index;
            break;
        }
    }
    return errorIndex;
}