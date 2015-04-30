#ifndef _INCLUDE_FUNCTIONS_H
#define _INCLUDE_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

/*Kbit arate*/
//#include "spatiallib.h"
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#include <math.h>
#include <sys/time.h>                // for gettimeofday()
#include <alsa/asoundlib.h>

#include <stdbool.h> /*for true and false*/

/*Reproducing*/
#include "./../include/spatiallib.h"
#include "./../include/superwavlib.h"

//Thread
#include <pthread.h>

long long current_timestamp();
void error(const char *msg);
int kbhit(void);
int changeFlag(int flag);

long long timeToStartInSeconds(int s);
long long timeToStartInMilisecons(int m);

void* playSuperWav(void *arguments);

#endif