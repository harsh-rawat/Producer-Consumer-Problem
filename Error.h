#ifndef ASSIGNMENT2_ERROR_H
#define ASSIGNMENT2_ERROR_H

void PrintErrorAndExit(int threadNumber, int errorNo);
void PrintMallocErrorAndExit(char* module, char* identityName, char* functionalIdentity);
void PrintSemInitErrorAndExit(char* module, char* identityName, char* functionalIdentity);
void PrintSemWaitErrorAndExit(char* module, char* identityName, char* functionalIdentity);
void PrintSemPostErrorAndExit(char* module, char* identityName, char* functionalIdentity);
void PrintOutputPrintErrorAndExit(char* module, char* identityName, char* functionalIdentity);

#endif
