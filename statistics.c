#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "statistics.h"

Stats* CreateStatistics(){
    Stats* stats = malloc(sizeof(Stats));
    stats->dequeueCount = 0;
    stats->enqueueCount = 0;
    stats->dequeueTime = 0.0;
    stats->enqueueTime = 0.0;

    sem_init(&stats->lock, 0, 1);

    return stats;
}

void UpdateEnqueueCount(Stats* stats, int count){
    sem_wait(&stats->lock);
    stats->enqueueCount = stats->enqueueCount + count;
    sem_post(&stats->lock);
}

void UpdateDequeueCount(Stats* stats, int count){
    sem_wait(&stats->lock);
    stats->dequeueCount = stats->dequeueCount + count;
    sem_post(&stats->lock);
}

void UpdateEnqueueTime(Stats* stats, clock_t startTime, clock_t endTime){
    sem_wait(&stats->lock);
    double time_spent = (double)(endTime - startTime)/CLOCKS_PER_SEC;
    stats->enqueueTime = stats->enqueueTime + time_spent;
    sem_post(&stats->lock);
}

void UpdateDequeueTime(Stats* stats, clock_t startTime, clock_t endTime){
    sem_wait(&stats->lock);
    double time_spent = (double)(endTime - startTime)/CLOCKS_PER_SEC;
    stats->dequeueTime = stats->dequeueTime + time_spent;
    sem_post(&stats->lock);
}

void PrintStatistics(Stats* stats, char* statsIdentity){
    sem_wait(&stats->lock);
    fprintf(stderr, "Statistics of %s -\n",statsIdentity);
    fprintf(stderr,"Enqueue count is %d\n", stats->enqueueCount);
    fprintf(stderr,"Dequeue count is %d\n", stats->dequeueCount);
    fprintf(stderr,"Enqueue time is %lf\n", stats->enqueueTime);
    fprintf(stderr,"Dequeue time is %lf\n\n", stats->dequeueTime);
    sem_post(&stats->lock);
}
