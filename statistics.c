/**
 * @author Harsh Rawat (hrawat2)
 * @author Sidharth Gurbani (gurbani)
 * */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "statistics.h"
#include "Error.h"

/**
 * @function CreateStatistics
 * @argument statsIdentity - Name associated with this stats struct
 * @description This method returns a new struct initialized with initial values of all the counters.
 * The access to this module is synchronized.
 * */
Stats* CreateStatistics(char* statsIdentity){
    // Allocate the memory for the stats struct. In case of any error, print an error message and return.
    Stats* stats = malloc(sizeof(Stats));
    if(stats == NULL){
        PrintMallocErrorAndExit(STATS_MODULE, statsIdentity, "CreateStatistics");
        return NULL;
    }

    // Set the name and initialize other counters with an initial value of 0.
    stats->statsIdentity = statsIdentity;
    stats->dequeueCount = 0;
    stats->enqueueCount = 0;
    stats->dequeueTime = 0.0;
    stats->enqueueTime = 0.0;

    // Initialize the lock semaphore which is used to synchronize access to this module.
    int retVal = sem_init(&stats->lock, 0, 1);

    if(retVal != 0)
        PrintSemInitErrorAndExit(STATS_MODULE, statsIdentity, "Lock");

    // return created stats struct
    return stats;
}

/**
 * @function UpdateEnqueueCount
 * @argument stats - stats struct used to maintain state for this module
 * @argument count - The count with which the counter needs to be incremented
 * @description Update the enqueue counter with the given count value
 * */
void UpdateEnqueueCount(Stats* stats, int count){
    int retVal;
    // Lock the method using semaphore
    retVal = sem_wait(&stats->lock);
    // In case of error, print the error message and exit
    if(retVal != 0) PrintSemWaitErrorAndExit(STATS_MODULE, stats->statsIdentity, "UpdateEnqueueCount");

    // Update the enqueue count
    stats->enqueueCount = stats->enqueueCount + count;

    // Release the method lock
    retVal = sem_post(&stats->lock);
    // In case of error, print the error message and exit
    if(retVal != 0) PrintSemPostErrorAndExit(STATS_MODULE,  stats->statsIdentity, "UpdateEnqueueCount");
}

/**
 * @function UpdateDequeueCount
 * @argument stats - stats struct used to maintain state for this module
 * @argument count - The count with which the counter needs to be incremented
 * @description Update the dequeue counter with the given count value
 * */
void UpdateDequeueCount(Stats* stats, int count){
    int retVal;
    // Lock the method using semaphore
    retVal = sem_wait(&stats->lock);
    // In case of error, print the error message and exit
    if(retVal != 0) PrintSemWaitErrorAndExit(STATS_MODULE,  stats->statsIdentity, "UpdateDequeueCount");

    // Update the dequeue count
    stats->dequeueCount = stats->dequeueCount + count;

    // Release the lock on this method
    retVal = sem_post(&stats->lock);
    // In case of error, print the error message and exit
    if(retVal != 0) PrintSemPostErrorAndExit(STATS_MODULE,  stats->statsIdentity, "UpdateDequeueCount");
}

/**
 * @function UpdateEnqueueTime
 * @argument stats - stats struct used to maintain state for this module
 * @argument startTime - Start time
 * @argument endTime - End time
 * @description Update the enqueue counter with the time elapsed
 * */
void UpdateEnqueueTime(Stats* stats, clock_t startTime, clock_t endTime){
    int retVal;
    // Lock the method using semaphore
    retVal = sem_wait(&stats->lock);
    // In case of error, print the error message and exit
    if(retVal != 0) PrintSemWaitErrorAndExit(STATS_MODULE,  stats->statsIdentity, "UpdateEnqueueTime");

    // Calculate the time elapsed between start and end. Update the counter with the same.
    double time_spent = (double)(endTime - startTime)/CLOCKS_PER_SEC;
    stats->enqueueTime = stats->enqueueTime + time_spent;

    // Release the method lock
    retVal = sem_post(&stats->lock);
    // In case of error, print the error message and exit
    if(retVal != 0) PrintSemPostErrorAndExit(STATS_MODULE,  stats->statsIdentity, "UpdateEnqueueTime");
}

/**
 * @function UpdateDequeueTime
 * @argument stats - stats struct used to maintain state for this module
 * @argument startTime - Start time
 * @argument endTime - End time
 * @description Update the dequeue counter with the time elapsed
 * */
void UpdateDequeueTime(Stats* stats, clock_t startTime, clock_t endTime){
    int retVal;
    // Lock the method using semaphore
    retVal = sem_wait(&stats->lock);
    // In case of error, print the error message and exit
    if(retVal != 0) PrintSemWaitErrorAndExit(STATS_MODULE,  stats->statsIdentity, "UpdateDequeueTime");

    // Calculate the time elapsed between start and end. Update the counter with the same.
    double time_spent = (double)(endTime - startTime)/CLOCKS_PER_SEC;
    stats->dequeueTime = stats->dequeueTime + time_spent;

    // Release the method lock
    retVal = sem_post(&stats->lock);
    // In case of error, print the error message and exit
    if(retVal != 0) PrintSemPostErrorAndExit(STATS_MODULE,  stats->statsIdentity, "UpdateDequeueTime");
}

/**
 * @function PrintStatistics
 * @argument stats - stats struct used to maintain state for this module
 * @description This method is used to print the stats maintained by this module
 * */
void PrintStatistics(Stats* stats){
    int retVal;
    // Lock the method using semaphore
    retVal = sem_wait(&stats->lock);
    // In case of error, print the error message and exit
    if(retVal != 0) PrintSemWaitErrorAndExit(STATS_MODULE,  stats->statsIdentity, "PrintStatistics");

    // Print the stats maintained by this module
    fprintf(stderr, "Statistics of %s -\n",stats->statsIdentity);
    fprintf(stderr,"Enqueue count is %d\n", stats->enqueueCount);
    fprintf(stderr,"Dequeue count is %d\n", stats->dequeueCount);
    fprintf(stderr,"Enqueue time is %lf\n", stats->enqueueTime);
    fprintf(stderr,"Dequeue time is %lf\n\n", stats->dequeueTime);

    // Release the method lock
    retVal = sem_post(&stats->lock);
    // In case of error, print the error message and exit
    if(retVal != 0) PrintSemPostErrorAndExit(STATS_MODULE,  stats->statsIdentity, "PrintStatistics");
}
