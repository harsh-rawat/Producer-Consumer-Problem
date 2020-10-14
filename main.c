#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "Queue.h"
#include "Threads.h"
#include "Error.h"

#define MAX_QUEUE_SIZE 10

static int findErrorIndex(int* retVals);

int main(){
    pthread_t reader_thread, munch1_thread, munch2_thread, writer_thread;

    Queue* reader_munch1_queue = CreateStringQueue(MAX_QUEUE_SIZE, "Reader-Munch1");
    Queue* munch1_munch2_queue = CreateStringQueue(MAX_QUEUE_SIZE, "Munch1-Munch2");
    Queue* munch2_writer_queue = CreateStringQueue(MAX_QUEUE_SIZE, "Munch2-Writer");

    Reader* reader = CreateReader(reader_munch1_queue);
    Munch1* munch1 = CreateMunch1(reader_munch1_queue, munch1_munch2_queue);
    Munch2* munch2 = CreateMunch2(munch1_munch2_queue, munch2_writer_queue);
    Writer* writer = CreateWriter(munch2_writer_queue);

    int thread_rets[4];
    thread_rets[0] = pthread_create(&reader_thread, NULL, StartReader, (void*) reader);
    thread_rets[1] = pthread_create(&munch1_thread, NULL, StartMunch1, (void*) munch1);
    thread_rets[2] = pthread_create(&munch2_thread, NULL, StartMunch2, (void*) munch2);
    thread_rets[3] = pthread_create(&writer_thread, NULL, StartWriter, (void*) writer);

    int errorIndex = findErrorIndex(thread_rets);
    if(errorIndex != -1){
        PrintErrorAndExit(errorIndex+1, thread_rets[errorIndex]);
    }

    pthread_join(reader_thread, NULL);
    pthread_join(munch1_thread, NULL);
    pthread_join(munch2_thread, NULL);
    pthread_join(writer_thread, NULL);

    PrintQueueStats(reader_munch1_queue);
    PrintQueueStats(munch1_munch2_queue);
    PrintQueueStats(munch2_writer_queue);

    exit(EXIT_SUCCESS);
}

static int findErrorIndex(int* retVals){
    int errorIndex = -1;
    for(int index = 0; index < 4; index++){
        if(retVals[index]){
            errorIndex = index;
            break;
        }
    }
    return errorIndex;
}