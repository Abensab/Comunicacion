#ifndef _INCLUDE_PIPE_H
#define _INCLUDE_PIPE_H

#include "./../funcions.h"

// some macros to make the code more understandable
//  regarding which pipe to use to a read/write operation
//
//  Parent: writes on P1_WRITE
//  Child:  reads from P1_READ

#define P1_READ     0
#define P1_WRITE    1

int* startPipeFileDescriptor();
void sendingFlag (int p[], int val);
int readFlag (int p[]);
void closePipeFileDescriptor(int p[]);

void closeReadingPipe(int p[]);
void closeWritingPipe(int p[]);

#endif