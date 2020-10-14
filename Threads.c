/**
 * @author Harsh Rawat (hrawat2)
 * @author Sidharth Gurbani (gurbani)
 * */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include "Threads.h"
#include "Error.h"

// Static utility functions
static int readLine(char* buffer);
static void copyLine(char* buffer, char* str, int len);
static void copyLineToQueue(Reader* reader, char* buffer);
static void signalEndOfExecutionByReader(Reader* reader, char* buffer, int freeBuffer);
static void replaceSpaceWithAsterisk(char* str);
static void convertLowerToUpperCase(char* str);

/**
 * @function CreateReader
 * @argument outputQueue - Shared queue between Reader-Munch1
 * @description
 * Initialize a Reader struct and return it
 * */
Reader* CreateReader(Queue* outputQueue){
    Reader* reader = malloc(sizeof(Reader));
    if(reader == NULL) PrintMallocErrorAndExit(THREADS_MODULE, READER, "CreateReader");
    reader->outputQueue = outputQueue;
    return reader;
}

/**
 * @function CreateMunch1
 * @argument inputQueue - Shared queue between Reader-Munch1
 * @argument outputQueue - Shared queue between Munch1-Munch2
 * @description
 * Initialize a Munch1 struct and return it
 * */
Munch1* CreateMunch1(Queue* inputQueue, Queue* outputQueue){
    Munch1* munch1 = malloc(sizeof(Munch1));
    if(munch1 == NULL) PrintMallocErrorAndExit(THREADS_MODULE, MUNCH1, "CreateMunch1");
    munch1->inputQueue = inputQueue;
    munch1->outputQueue = outputQueue;
    return munch1;
}

/**
 * @function CreateMunch2
 * @argument inputQueue - Shared queue between Munch1-Munch2
 * @argument outputQueue - Shared queue between Munch2-Writer
 * @description
 * Initialize a Munch2 struct and return it
 * */
Munch2* CreateMunch2(Queue* inputQueue, Queue* outputQueue){
    Munch2* munch2 = malloc(sizeof(Munch2));
    if(munch2 == NULL) PrintMallocErrorAndExit(THREADS_MODULE, MUNCH2, "CreateMunch2");
    munch2->inputQueue = inputQueue;
    munch2->outputQueue = outputQueue;
    return munch2;
}

/**
 * @function CreateWriter
 * @argument inputQueue - Shared queue between Munch2-Writer
 * @description
 * Initialize a Writer struct and return it
 * */
Writer* CreateWriter(Queue* inputQueue){
    Writer* writer = malloc(sizeof(Writer));
    if(writer == NULL) PrintMallocErrorAndExit(THREADS_MODULE, WRITER, "CreateWriter");
    writer->inputQueue = inputQueue;
    writer->stringsProcessedCount = 0;
    return writer;
}

/**
 * @function StartReader
 * @argument ptr - Reader struct passed via create_thread
 * @description
 * Starts the reader operation in a separate thread.
 * Reads from stdin and fills its buffer. If the line length exceeds max length then buffer is flushed.
 * */
void* StartReader(void* ptr){
    Reader* reader = (Reader*) ptr;

    while(1){
        // Allocate buffer for each new line. This buffer will be freed and the string will be copied to a new proper sized buffer.
        char* buffer = malloc(sizeof(char) * MAX_BUFFER_SIZE);
        // In case of error, print error message and exit with failure code
        if(buffer == NULL) PrintMallocErrorAndExit(THREADS_MODULE, READER, "Buffer");

        // Read the line from stdin
        int retVal = readLine(buffer);
        if(retVal == -1){ // retVal = -1 means buffer overflow, so skip this line
            free(buffer);
            continue;
        } else if(retVal == -2){// retVal = -2 means EOF is received and there is no data in buffer. Signal end directly.
            signalEndOfExecutionByReader(reader, buffer, 1);
            break;
        } else if(retVal == -3){// retVal = -3 means EOF is received and there is some data to be copied in buffer. Copy data then signal end.
            copyLineToQueue(reader, buffer);
            signalEndOfExecutionByReader(reader, buffer, 0);
            break;
        } else if(retVal == -4){// retVal = -4 means EOF is received after the current line overflow the buffer. So skip line and signal end.
            signalEndOfExecutionByReader(reader, buffer, 1);
            break;
        }
        // In case of normal execution, copy the contents to an appropriately sized string and enqueue it.
        copyLineToQueue(reader, buffer);
    }

    pthread_exit(NULL);
}

/**
 * @function StartMunch1
 * @argument ptr - Munch1 struct
 * @description
 * This method runs in its own thread and performs Munch1 functionality i.e. converts space to *
 * Adds the converted string to shared queue between Munch1 and Munch2
 * */
void* StartMunch1(void* ptr){
    Munch1* munch1 = (Munch1*) ptr;

    while(1){
        // Dequeue a string from Reader-Munch1 queue
        char* str = DequeueString(munch1->inputQueue);
        // EndOfExecution is signalled by NULL being passed through the pipeline.
        if(str == NULL){
            // Propagate EndOfExecution to next stage and terminate this thread
            EnqueueString(munch1->outputQueue, NULL);
            break;
        }
        // Convert space to *
        replaceSpaceWithAsterisk(str);
        // Enqueue this string to next stage queue
        EnqueueString(munch1->outputQueue, str);
    }

    pthread_exit(NULL);
}

/**
 * @function StartMunch2
 * @argument ptr - Munch2 struct
 * @description
 * This method runs in its own thread and performs Munch2 functionality i.e. converts lower case to upper case
 * Adds the converted string to shared queue between Munch2 and Writer
 * */
void* StartMunch2(void* ptr){
    Munch2* munch2 = (Munch2*) ptr;

    while(1){
        // Dequeue a string from Munch1-Munch2 queue
        char* str = DequeueString(munch2->inputQueue);
        // EndOfExecution is signalled by NULL being passed through the pipeline.
        if(str == NULL){
            // Propagate EndOfExecution to next stage and terminate this thread
            EnqueueString(munch2->outputQueue, NULL);
            break;
        }
        // Convert lower case to upper case
        convertLowerToUpperCase(str);
        // Enqueue string to Munch2-Writer queue
        EnqueueString(munch2->outputQueue, str);
    }

    pthread_exit(NULL);
}

/**
 * @function StartWriter
 * @argument ptr - Writer struct
 * @description
 * This method runs in its own thread and writes the data to stdout. It also maintains a count of strings processed.
 * */
void* StartWriter(void* ptr){
    Writer* writer = (Writer*) ptr;

    int retVal;
    while(1){
        // Dequeue a string from Munch2-Writer queue
        char* str = DequeueString(writer->inputQueue);
        // EndOfExecution is signalled by NULL being passed through the pipeline.
        if(str == NULL){
            // Print the total number of strings processed and then terminate this thread.
            retVal = printf("Writer processed %d strings!\n\n",writer->stringsProcessedCount);
            if(retVal < 0) PrintOutputPrintErrorAndExit(THREADS_MODULE, WRITER, "Processed Count");
            break;
        }
        // Print the string to stdout
        retVal = printf("%s\n",str);
        if(retVal < 0) PrintOutputPrintErrorAndExit(THREADS_MODULE, WRITER, "Processed-String");

        // Increment the count of strings which have been processed
        writer->stringsProcessedCount = writer->stringsProcessedCount + 1;
        free(str);
    }

    pthread_exit(NULL);
}

/**
 * @function readLine
 * @argument buffer - The buffer in which the data is stored
 * @description
 * This method reads a line from stdin using fgetc. If the total length of string becomes equal to max buffer size, then we ignore that line.
 *
 * A return value of -1 means that the buffer was flushed due to length of line exceeding the max size
 * A return value of -2 means that EOF has been reached with empty buffer
 * A return value of -3 means that EOF has been reached with some value in buffer
 * A return value of -4 means that EOF was reached after line length exceeded Max length
 * A positive return value means normal execution with data in buffer which needs to be copied to queue
 * */
static int readLine(char* buffer){
    int len = 0, eof = 0, retVal = 0;
    char ch;
    while(1){
        // Read a character from stdin
        ch = fgetc(stdin);
        // If the total length until now exceeds MAX_BUFFER_SIZE then do not update the buffer. Wait for EOF or newline.
        if(len > MAX_BUFFER_SIZE){
            // In case of EOF being received, we will use eof variable as a flag
            if(ch == EOF) {
                eof = 1;
                break;
            }
            else if(ch == '\n') { // In case len > MAX_BUFFER_SIZE wait for len to break the read loop
                break;
            }
        } else {
            // If the total len is less that max buffer size then execute this
            if(ch == EOF){ // If EOF is received then check the len to set the appropriate return value
                if(len == 0){
                    len = -2;
                } else {
                    len = -3;
                }
                break;
            } else if(ch == '\n'){ // If this is new line char then ignore it and break the read loop
                break;
            }
            // In case of normal character with len < MAX_BUFFER_SIZE, update the buffer
            buffer[len++] = ch;
        }
    }

    // In case the length is greater than max buffer size, set the appropriate return value depending on eof flag
    if(len > MAX_BUFFER_SIZE){
        if(eof == 0)
            len = -1;
        else
            len = -4;
        retVal = fprintf(stderr, "Current line's length exceeded the max size of buffer. Skipping it.\n");
        if(retVal < 0) PrintOutputPrintErrorAndExit(THREADS_MODULE, READER, "STDERR-Buffer-Exceeded");
    }

    // return the response code
    return len;
}

/**
 * @function copyLine
 * @argument buffer - Buffer in which data was being stored i.e. input buffer
 * @argument str - The buffer into which the data has to be copied i.e. output buffer
 * @argument len - length of the input string
 * @description
 * This method copies the data from input buffer to output buffer
 * */
static void copyLine(char* buffer, char* str, int len){
    for(int i =0; i < len; i++){
        str[i] = buffer[i];
    }
}

/**
 * @function copyLineToQueue
 * @argument reader - Reader struct
 * @argument buffer - Buffer in which data was being stored
 * @description
 * This method allocates a new string buffer which is equal to the length of the input string.
 * The contents of the buffer are copied in this new buffer which is then enqueued on Reader-Munch1 queue
 * */
static void copyLineToQueue(Reader* reader, char* buffer){
    // Find the length of the original string
    int len = strlen(buffer);
    // Allocate a new buffer which is equal to the length of the string
    char* str = calloc(len, sizeof(char));
    if(str == NULL) PrintMallocErrorAndExit(THREADS_MODULE, READER, "calloc-reallocateString");

    // Copy the contents of original buffer into new buffer
    copyLine(buffer, str, len);
    // Free the original buffer
    free(buffer);
    // Enqueue the string in Reader-Munch1 queue
    EnqueueString(reader->outputQueue, str);
}

/**
 * @function signalEndOfExecutionByReader
 * @argument reader - Reader struct
 * @argument buffer - Buffer in which data was being stored
 * @argument freeBuffer - If 1 then free the buffer else ignore it
 * @description
 * This function signals the end of execution from Reader by passing a NULL value in the Reader-Munch1 queue
 * */
static void signalEndOfExecutionByReader(Reader* reader, char* buffer, int freeBuffer){
    if(freeBuffer){
        free(buffer);
    }
    // EndOfExecution is signalled by NULL being passed through the pipeline.
    // Therefore, pass NULL to Reader-Munch1 queue
    EnqueueString(reader->outputQueue, NULL);
}

/**
 * @function replaceSpaceWithAsterisk
 * @argument str - The string to be processed
 * @description
 * This function converts spaces in a string to *
 * */
static void replaceSpaceWithAsterisk(char* str){
    int index, len = strlen(str);
    // Iterate over the length of string
    for(index = 0; index < len; index++){
        char ch = str[index];
        // If current character is space then change it to *
        if(ch == ' '){
            str[index] = '*';
        }
    }
}

/**
 * @function convertLowerToUpperCase
 * @argument str - The string to be processed
 * @description
 * This function converts a lowercase string to upper case
 * */
static void convertLowerToUpperCase(char* str){
    int index, len = strlen(str);
    // Iterate over the length of string
    for(index = 0; index < len; index++){
        char ch = str[index];
        // If the current character is lower case then change it to upper case
        if(islower(ch) > 0){
            str[index] = toupper(str[index]);
        }
    }
}
