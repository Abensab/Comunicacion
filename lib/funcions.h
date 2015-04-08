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

long long current_timestamp();
void error(const char *msg);
int kbhit(void);

#endif