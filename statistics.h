#ifndef ASSIGNMENT2_STATISTICS_H
#define ASSIGNMENT2_STATISTICS_H

#include <semaphore.h>

#define STATS_MODULE "Statistics"

typedef struct {
    char* statsIdentity;

    int enqueueCount;
    int dequeueCount;
    double enqueueTime;
    double dequeueTime;

    sem_t lock;
} Stats;

Stats* CreateStatistics(char* statsIdentity);
void UpdateEnqueueCount(Stats* stats, int count);
void UpdateDequeueCount(Stats* stats, int count);
void UpdateEnqueueTime(Stats* stats, clock_t startTime, clock_t endTime);
void UpdateDequeueTime(Stats* stats, clock_t startTime, clock_t endTime);
void PrintStatistics(Stats* stats);

#endif
