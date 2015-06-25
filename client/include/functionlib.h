#ifndef _INCLUDE_FUNCTIONSLIB_H
#define _INCLUDE_FUNCTIONSLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

/*Kbit arate*/
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#include <math.h>
// for gettimeofday()
#include <sys/time.h>
//#include <alsa/asoundlib.h>

//for true and false
#include <stdbool.h>
#define TRUE   1
#define FALSE  0

//Thread
#include <pthread.h>

long long current_timestamp();
void error(const char *msg);
int kbhit(void);
char **strsplit(const char* str, const char* delim, size_t* numtokens);

#endif