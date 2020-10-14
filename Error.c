#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Error.h"

void PrintErrorAndExit(int threadNumber, int errorNo){
    fprintf(stderr, "Thread %d could not be created. Error : %s\nExiting!\n", threadNumber, strerror(errorNo));
    exit(EXIT_FAILURE);
}

void PrintMallocErrorAndExit(char* module, char* identityName, char* functionalIdentity){
    fprintf(stderr, "Memory could not be allocated using malloc/calloc for %s:%s:%s. Exiting!\n", module, identityName, functionalIdentity);
    exit(EXIT_FAILURE);
}

void PrintSemInitErrorAndExit(char* module, char* identityName, char* functionalIdentity){
    fprintf(stderr, "Error during semaphore initialization in %s:%s:%s. Exiting!\n", module, identityName, functionalIdentity);
    exit(EXIT_FAILURE);
}

void PrintSemWaitErrorAndExit(char* module, char* identityName, char* functionalIdentity){
    fprintf(stderr, "Error during semaphore wait in %s:%s:%s. Exiting!\n", module, identityName, functionalIdentity);
    exit(EXIT_FAILURE);
}

void PrintSemPostErrorAndExit(char* module, char* identityName, char* functionalIdentity){
    fprintf(stderr, "Error during semaphore post in %s:%s:%s. Exiting!\n", module, identityName, functionalIdentity);
    exit(EXIT_FAILURE);
}

void PrintOutputPrintErrorAndExit(char* module, char* identityName, char* functionalIdentity){
    fprintf(stderr, "Error occurred while printing string in %s:%s:%s. Exiting!\n", module, identityName, functionalIdentity);
    exit(EXIT_FAILURE);
}

