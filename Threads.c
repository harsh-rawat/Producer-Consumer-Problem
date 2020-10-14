/**
 * @author Harsh Rawat, harsh-rawat, hrawat2
 * @author Sidharth Gurbani, gurbani, gurbani
 * */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include "Threads.h"
#include "Error.h"

// Static utility functions
static void readLine(char* buffer, int* response);
static void copyLine(char* buffer, char* str, int len);
static void copyLineToQueue(Reader* reader, char* buffer, int len);
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
    if(reader == NULL) {
        PrintMallocErrorAndExit(THREADS_MODULE, READER, "CreateReader");
        return NULL;
    }
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
    if(munch1 == NULL) {
        PrintMallocErrorAndExit(THREADS_MODULE, MUNCH1, "CreateMunch1");
        return NULL;
    }
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
    if(munch2 == NULL) {
        PrintMallocErrorAndExit(THREADS_MODULE, MUNCH2, "CreateMunch2");
        return NULL;
    }
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
    if(writer == NULL) {
        PrintMallocErrorAndExit(THREADS_MODULE, WRITER, "CreateWriter");
        return NULL;
    }
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
        if(buffer == NULL) {
            PrintMallocErrorAndExit(THREADS_MODULE, READER, "Buffer");
            return NULL;
        }

        // Allocate memory for response of reading line
        int* response = malloc(sizeof(int)*2);
        // Read the line from stdin
        readLine(buffer, response);

        if(response[0] == -1){ // response = -1 means buffer overflow, so skip this line
            free(buffer);
            continue;
        } else if(response[0] == -2){// response = -2 means EOF is received and there is no data in buffer. Signal end directly.
            signalEndOfExecutionByReader(reader, buffer, 1);
            break;
        } else if(response[0] == -3){// response = -3 means EOF is received and there is some data to be copied in buffer. Copy data then signal end.
            copyLineToQueue(reader, buffer, response[1]);
            signalEndOfExecutionByReader(reader, NULL, 0);
            break;
        } else if(response[0] == -4){// response = -4 means EOF is received after the current line overflow the buffer. So skip line and signal end.
            signalEndOfExecutionByReader(reader, buffer, 1);
            break;
        }

        // In case of normal execution, copy the contents to an appropriately sized string and enqueue it.
        copyLineToQueue(reader, buffer, response[1]);
        free(response);
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
 * @argument response - pointer to an integer array which will hold the response
 * @description
 * This method reads a line from stdin using fgetc. If the total length of string becomes equal to max buffer size, then we ignore that line.
 *
 * The response is returned by this method using the response array taken as param.
 * The first index corresponds to status and the second index corresponds to length of the string
 *
 * For Status-
 * A status value of -1 means that the buffer was flushed due to length of line exceeding the max size
 * A status value of -2 means that EOF has been reached with empty buffer
 * A status value of -3 means that EOF has been reached with some value in buffer
 * A status value of -4 means that EOF was reached after line length exceeded Max length. Therefore skip the line.
 * A status value of 0 means normal execution
 *
 * When buffer contains some value then we append '\0' at the end.
 * Second index of response is set to length for status 0, -3.
 *
 * Explanation for newline case-
 * response would be [0,0] which means normal execution and length of 0.
 * In the copyLine method, a null string would be created which will be enqueued.
 * */
static void readLine(char* buffer, int* response){
    int len = 0, eof = 0, retVal = 0;
    char ch;
    while(1){
        // Read a character from stdin
        ch = fgetc(stdin);

        // If the total length until now exceeds MAX_BUFFER_SIZE -1 then do not update the buffer. Wait for EOF or newline.
        // Consider that MAX_BUFFER_SIZE is 10. Then when len == 10 then 10 characters would have been placed in buffer and there won't be space for last '\0'
        // Hence this would be an overflow
        if(len >= MAX_BUFFER_SIZE){
            // In case of EOF being received, we will use eof variable as a flag
            if(ch == EOF) {
                eof = 1;
                break;
            }
            else if(ch == '\n') { // In case len >= MAX_BUFFER_SIZE wait for len to break the read loop
                break;
            }
        } else {
            // If the total len is less that max buffer size then execute this
            if(ch == EOF){ // If EOF is received then check the len to set the appropriate return value
                if(len == 0){
                    response[0] = -2;
                } else {
                    buffer[len] = '\0';
                    response[0] = -3;
                    response[1] = len;
                }
                break;
            } else if(ch == '\n'){ // If this is new line char then ignore it and break the read loop
                buffer[len] = '\0';
                response[0] = 0;
                response[1] = len;
                break;
            }
            // In case of normal character with len < MAX_BUFFER_SIZE, update the buffer
            buffer[len++] = ch;
        }
    }

    // In case of buffer overflow, set the appropriate return value depending on eof flag
    if(len >= MAX_BUFFER_SIZE){
        if(eof == 0) {
            response[0] = -1;
            response[1] = 0;
        }
        else {
            response[0] = -4;
            response[1] = 0;
        }
        retVal = fprintf(stderr, "Current line's length exceeded the max size of buffer. Skipping it.\n");
        if(retVal < 0) PrintOutputPrintErrorAndExit(THREADS_MODULE, READER, "STDERR-Buffer-Exceeded");
    }
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
    int i = 0;
    while(i < len){
        str[i] = buffer[i];
        i = i + 1;
    }
    // Last character of the line should be null
    str[len] = '\0';
}

/**
 * @function copyLineToQueue
 * @argument reader - Reader struct
 * @argument buffer - Buffer in which data was being stored
 * @argument len - length of the input string
 * @description
 * This method allocates a new string buffer which is equal to the length of the input string.
 * The contents of the buffer are copied in this new buffer which is then enqueued on Reader-Munch1 queue
 * */
static void copyLineToQueue(Reader* reader, char* buffer, int len){
    if(buffer == NULL) return;

    // Allocate a new buffer which is equal to the length  of the string + 1. Extra 1 is for the null character at the end.
    char* str = calloc(len+1, sizeof(char));
    if(str == NULL) {
        free(buffer);
        PrintMallocErrorAndExit(THREADS_MODULE, READER, "calloc-reallocateString");
        return;
    }

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
