#ifndef CONNECTION
#define CONNECTION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>

long long current_timestamp();
void error(const char *msg);
int playSuperWav();
void  writeTimeDelay(long long delayTime);
void dostuff (int sock);
int serverConnection(int portNumber);
int clientConnection(char *address, int portNumber);

#endif