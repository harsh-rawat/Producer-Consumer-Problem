#ifndef ASSIGNMENT2_STATISTICS_H
#define ASSIGNMENT2_STATISTICS_H

#include <semaphore.h>

typedef struct {
    int enqueueCount;
    int dequeueCount;
    double enqueueTime;
    double dequeueTime;

    sem_t lock;
} Stats;

Stats* CreateStatistics();
void UpdateEnqueueCount(Stats* stats, int count);
void UpdateDequeueCount(Stats* stats, int count);
void UpdateEnqueueTime(Stats* stats, clock_t startTime, clock_t endTime);
void UpdateDequeueTime(Stats* stats, clock_t startTime, clock_t endTime);
void PrintStatistics(Stats* stats, char* statsIdentity);

#endif
