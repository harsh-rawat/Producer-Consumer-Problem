/**
 * @author Harsh Rawat, harsh-rawat, hrawat2
 * @author Sidharth Gurbani, gurbani, gurbani
 *
 * @description
 * This module is used to collect all the error handling functionality at a single place.
 * Additionally, if required, complex error handling can be incorporated in this module without impacting other modules.
 *
 * @functions
 * PrintErrorAndExit - Used for cases when any thread wants to print an error and exit. The error number is converted to its message.
 * PrintMallocErrorAndExit - Used for cases when we cannot allocate memory using malloc/calloc
 * PrintSemInitErrorAndExit - Used for cases when we receive an error in sem_init
 * PrintSemWaitErrorAndExit - Used for cases when we receive an error in sem_wait
 * PrintSemPostErrorAndExit - Used for cases when we receive an error in sem_post
 * PrintOutputPrintErrorAndExit - Used for cases when we receive an error while printing to stdout or stderr
 *
 * */

#ifndef ASSIGNMENT2_ERROR_H
#define ASSIGNMENT2_ERROR_H

void PrintErrorAndExit(int threadNumber, int errorNo);
void PrintMallocErrorAndExit(char* module, char* identityName, char* functionalIdentity);
void PrintSemInitErrorAndExit(char* module, char* identityName, char* functionalIdentity);
void PrintSemWaitErrorAndExit(char* module, char* identityName, char* functionalIdentity);
void PrintSemPostErrorAndExit(char* module, char* identityName, char* functionalIdentity);
void PrintOutputPrintErrorAndExit(char* module, char* identityName, char* functionalIdentity);

#endif
