CC = gcc
CFLAGS  = -g -Wall -Wextra

LIBS = ./../../lib/fft/fftGS.c -lm

NEONLIBS = -march=armv7 -mfpu=neon

all:
	$(CC) -O3 comparation.c $(LIBS) $(NEONLIBS) -o comparation.o

debug:
	$(CC) -O3 $(CFLAGS) comparation.c $(LIBS) $(NEONLIBS) -o comparation.o	

clean:
	$(RM) -r *.o *.o.* *~
