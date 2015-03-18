#ifndef SPATIALLIB_H
#define SPATIALLIB_H

#include <stdio.h>   
#include <stdlib.h>
#include <string.h>


unsigned char * newWavdieciseis(char nombre[]);
unsigned char * newWav2ch(char nombreL[], char nombreR[]);
int OpenWav(unsigned char **pwav, char nombre[]);
int OpenWavConvert32(unsigned char **pwav, char nombre[]);
void freeWav(unsigned char *pwav);

#endif


