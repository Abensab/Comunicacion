CC = gcc
CFLAGS  = -g -Wall -Wextra

LIBS = -lm -lfftw3

NEONLIBS = -march=armv7 -mfpu=neon -lm

FFTW = /home/myhay/ClionProjects/Comunicacion/bin/fftw/fftw-3.3.4/.libs/libfftw3.a

all:
	$(CC) -O3 convolutionTest.c  ./../../lib/convolution/convolution.c $(LIBS) $(FFTW) -o test.o

debug:
	$(CC) -O3 $(CFLAGS) convolutionTest.c  ./../../lib/convolution/convolution.c $(LIBS) -o test.o

clean:
	$(RM) -r *.o *.o.* *~
