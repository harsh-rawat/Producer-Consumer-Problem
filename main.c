#include <pthread.h>
#include <stdlib.h>
#include "Queue.h"
#include "Threads.h"

int main(){
    pthread_t reader_thread, munch1_thread, munch2_thread, writer_thread;

    Queue* reader_munch1_queue = CreateStringQueue(10, "Reader-Munch1");
    Queue* munch1_munch2_queue = CreateStringQueue(10, "Munch1-Munch2");
    Queue* munch2_writer_queue = CreateStringQueue(10, "Munch2-Writer");

    Reader* reader = CreateReader(reader_munch1_queue);
    Munch1* munch1 = CreateMunch1(reader_munch1_queue, munch1_munch2_queue);
    Munch2* munch2 = CreateMunch2(munch1_munch2_queue, munch2_writer_queue);
    Writer* writer = CreateWriter(munch2_writer_queue);

    pthread_create(&reader_thread, NULL, StartReader, (void*) reader);
    pthread_create(&munch1_thread, NULL, StartMunch1, (void*) munch1);
    pthread_create(&munch2_thread, NULL, StartMunch2, (void*) munch2);
    pthread_create(&writer_thread, NULL, StartWriter, (void*) writer);

    pthread_join(reader_thread, NULL);
    pthread_join(munch1_thread, NULL);
    pthread_join(munch2_thread, NULL);
    pthread_join(writer_thread, NULL);

    PrintQueueStats(reader_munch1_queue);
    PrintQueueStats(munch1_munch2_queue);
    PrintQueueStats(munch2_writer_queue);

    exit(0);
}
