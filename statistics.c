#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "statistics.h"
#include "Error.h"

Stats* CreateStatistics(char* statsIdentity){
    Stats* stats = malloc(sizeof(Stats));
    if(stats == NULL){
        PrintMallocErrorAndExit(STATS_MODULE, statsIdentity, "CreateStatistics");
    }

    stats->statsIdentity = statsIdentity;
    stats->dequeueCount = 0;
    stats->enqueueCount = 0;
    stats->dequeueTime = 0.0;
    stats->enqueueTime = 0.0;

    int retVal = sem_init(&stats->lock, 0, 1);

    if(retVal != 0)
        PrintSemInitErrorAndExit(STATS_MODULE, statsIdentity, "Lock");

    return stats;
}

void UpdateEnqueueCount(Stats* stats, int count){
    int retVal;
    retVal = sem_wait(&stats->lock);
    if(retVal != 0) PrintSemWaitErrorAndExit(STATS_MODULE, stats->statsIdentity, "UpdateEnqueueCount");

    stats->enqueueCount = stats->enqueueCount + count;

    retVal = sem_post(&stats->lock);
    if(retVal != 0) PrintSemPostErrorAndExit(STATS_MODULE,  stats->statsIdentity, "UpdateEnqueueCount");
}

void UpdateDequeueCount(Stats* stats, int count){
    int retVal;
    retVal = sem_wait(&stats->lock);
    if(retVal != 0) PrintSemWaitErrorAndExit(STATS_MODULE,  stats->statsIdentity, "UpdateDequeueCount");

    stats->dequeueCount = stats->dequeueCount + count;

    retVal = sem_post(&stats->lock);
    if(retVal != 0) PrintSemPostErrorAndExit(STATS_MODULE,  stats->statsIdentity, "UpdateDequeueCount");
}

void UpdateEnqueueTime(Stats* stats, clock_t startTime, clock_t endTime){
    int retVal;
    retVal = sem_wait(&stats->lock);
    if(retVal != 0) PrintSemWaitErrorAndExit(STATS_MODULE,  stats->statsIdentity, "UpdateEnqueueTime");

    double time_spent = (double)(endTime - startTime)/CLOCKS_PER_SEC;
    stats->enqueueTime = stats->enqueueTime + time_spent;

    retVal = sem_post(&stats->lock);
    if(retVal != 0) PrintSemPostErrorAndExit(STATS_MODULE,  stats->statsIdentity, "UpdateEnqueueTime");
}

void UpdateDequeueTime(Stats* stats, clock_t startTime, clock_t endTime){
    int retVal;
    retVal = sem_wait(&stats->lock);
    if(retVal != 0) PrintSemWaitErrorAndExit(STATS_MODULE,  stats->statsIdentity, "UpdateDequeueTime");

    double time_spent = (double)(endTime - startTime)/CLOCKS_PER_SEC;
    stats->dequeueTime = stats->dequeueTime + time_spent;

    retVal = sem_post(&stats->lock);
    if(retVal != 0) PrintSemPostErrorAndExit(STATS_MODULE,  stats->statsIdentity, "UpdateDequeueTime");
}

void PrintStatistics(Stats* stats){
    int retVal;
    retVal = sem_wait(&stats->lock);
    if(retVal != 0) PrintSemWaitErrorAndExit(STATS_MODULE,  stats->statsIdentity, "PrintStatistics");

    fprintf(stderr, "Statistics of %s -\n",stats->statsIdentity);
    fprintf(stderr,"Enqueue count is %d\n", stats->enqueueCount);
    fprintf(stderr,"Dequeue count is %d\n", stats->dequeueCount);
    fprintf(stderr,"Enqueue time is %lf\n", stats->enqueueTime);
    fprintf(stderr,"Dequeue time is %lf\n\n", stats->dequeueTime);

    retVal = sem_post(&stats->lock);
    if(retVal != 0) PrintSemPostErrorAndExit(STATS_MODULE,  stats->statsIdentity, "PrintStatistics");
}
