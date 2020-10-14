/**
 * @author Harsh Rawat (hrawat2)
 * @author Sidharth Gurbani (gurbani)
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Error.h"

/**
 * @function PrintErrorAndExit
 * @argument threadNumber - Index of the thread which encountered the error
 * @argument errorNo - The error number returned by the system call
 * @description Print the error to stderr and exit with failure code. Converts error number to string.
 * */
void PrintErrorAndExit(int threadNumber, int errorNo){
    fprintf(stderr, "Thread %d could not be created. Error : %s\nExiting!\n", threadNumber, strerror(errorNo));
    exit(EXIT_FAILURE);
}

/**
 * @function PrintMallocErrorAndExit
 * @argument module - Module which called this method. Example- 'Queue'
 * @argument identityName - Name of the identity which called this function. Example- 'Munch1-Munch2'
 * @argument functionalIdentity - Name of the function which called this function. Example- 'EnqueueString'
 * @description Print the error message to stderr and exit with failure code. Used for malloc/calloc errors.
 * */
void PrintMallocErrorAndExit(char* module, char* identityName, char* functionalIdentity){
    fprintf(stderr, "Memory could not be allocated using malloc/calloc for %s:%s:%s. Exiting!\n", module, identityName, functionalIdentity);
    exit(EXIT_FAILURE);
}

/**
 * @function PrintSemInitErrorAndExit
 * @argument module - Module which called this method. Example- 'Queue'
 * @argument identityName - Name of the identity which called this function. Example- 'Munch1-Munch2'
 * @argument functionalIdentity - Name of the function which called this function. Example- 'EnqueueString'
 * @description Print the error message to stderr and exit with failure code. Used for sem_init errors.
 * */
void PrintSemInitErrorAndExit(char* module, char* identityName, char* functionalIdentity){
    fprintf(stderr, "Error during semaphore initialization in %s:%s:%s. Exiting!\n", module, identityName, functionalIdentity);
    exit(EXIT_FAILURE);
}

/**
 * @function PrintSemWaitErrorAndExit
 * @argument module - Module which called this method. Example- 'Queue'
 * @argument identityName - Name of the identity which called this function. Example- 'Munch1-Munch2'
 * @argument functionalIdentity - Name of the function which called this function. Example- 'EnqueueString'
 * @description Print the error message to stderr and exit with failure code. Used for sem_wait errors.
 * */
void PrintSemWaitErrorAndExit(char* module, char* identityName, char* functionalIdentity){
    fprintf(stderr, "Error during semaphore wait in %s:%s:%s. Exiting!\n", module, identityName, functionalIdentity);
    exit(EXIT_FAILURE);
}

/**
 * @function PrintSemPostErrorAndExit
 * @argument module - Module which called this method. Example- 'Queue'
 * @argument identityName - Name of the identity which called this function. Example- 'Munch1-Munch2'
 * @argument functionalIdentity - Name of the function which called this function. Example- 'EnqueueString'
 * @description Print the error message to stderr and exit with failure code. Used for sem_post errors.
 * */
void PrintSemPostErrorAndExit(char* module, char* identityName, char* functionalIdentity){
    fprintf(stderr, "Error during semaphore post in %s:%s:%s. Exiting!\n", module, identityName, functionalIdentity);
    exit(EXIT_FAILURE);
}

/**
 * @function PrintOutputPrintErrorAndExit
 * @argument module - Module which called this method. Example- 'Queue'
 * @argument identityName - Name of the identity which called this function. Example- 'Munch1-Munch2'
 * @argument functionalIdentity - Name of the function which called this function. Example- 'EnqueueString'
 * @description Print the error message to stderr and exit with failure code. Used for printf, fprintf, etc errors.
 * */
void PrintOutputPrintErrorAndExit(char* module, char* identityName, char* functionalIdentity){
    fprintf(stderr, "Error occurred while printing string in %s:%s:%s. Exiting!\n", module, identityName, functionalIdentity);
    exit(EXIT_FAILURE);
}

