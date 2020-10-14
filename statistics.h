/**
 * @author Harsh Rawat (hrawat2)
 * @author Sidharth Gurbani (gurbani)
 *
 * @description
 * This module is used to maintain the stats associated with any queue.
 * A separate module is beneficial as new functionality can be added as required without any impact to other modules.
 *
 * @functions
 * CreateStatistics - Create a stats struct which will be used to perform the functionality provided in this module
 * UpdateEnqueueCount - Update the counter for enqueue ops
 * UpdateDequeueCount - Update the counter for dequeue ops
 * UpdateEnqueueTime - Update the time counter for enqueue op
 * UpdateDequeueTime - Update the time counter for dequeue op
 * PrintStatistics - Print the stats maintained in this module
 * */

#ifndef ASSIGNMENT2_STATISTICS_H
#define ASSIGNMENT2_STATISTICS_H

#include <semaphore.h>

#define STATS_MODULE "Statistics"

typedef struct {
    // Name associated with this struct
    char* statsIdentity;

    // Enqueue count
    int enqueueCount;
    // Dequeue count
    int dequeueCount;
    // Enqueue time
    double enqueueTime;
    // Dequeue time
    double dequeueTime;

    // Semaphore to synchronize access to this module
    sem_t lock;
} Stats;

Stats* CreateStatistics(char* statsIdentity);
void UpdateEnqueueCount(Stats* stats, int count);
void UpdateDequeueCount(Stats* stats, int count);
void UpdateEnqueueTime(Stats* stats, clock_t startTime, clock_t endTime);
void UpdateDequeueTime(Stats* stats, clock_t startTime, clock_t endTime);
void PrintStatistics(Stats* stats);

#endif
