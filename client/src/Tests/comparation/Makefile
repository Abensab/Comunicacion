CC = gcc
CFLAGS  = -g -Wall -Wextra

LIBS = ./../../lib/fft/fftGS.c -lm

NEONLIBS = -march=armv7 -mfpu=neon -lm

all:
	$(CC) -O3 comparation.c $(LIBS) -o comparation.o

debug:
	$(CC) -O3 $(CFLAGS) comparation.c $(LIBS) -o comparation.o

ARM:
	$(CC) -O3 comparation.c $(NEONLIBS) -o comparation.o

ARMdebug:
	$(CC) -O3 $(CFLAGS) comparation.c $(NEONLIBS) -o comparation.o


clean:
	$(RM) -r *.o *.o.* *~
