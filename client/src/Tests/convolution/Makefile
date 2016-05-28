CC = gcc
CFLAGS  = -g -Wall -Wextra

LIBS = -lm

NEONLIBS = -march=armv7 -mfpu=neon -lm

all:
	$(CC) -O3 testConvolution.c  ./../../lib/convolution/convolution.c $(LIBS) -o test.o

debug:
	$(CC) -O3 $(CFLAGS) testConvolution.c  ./../../lib/convolution/convolution.c $(LIBS) -o test.o

ARM:
	$(CC) -O3 $(CFLAGS) testConvolution.c ./../../lib/convolution/convolution.c $(NEONLIBS) -o testNEON.o

ARMdebug:
    $(CC) -O3 $(CFLAGS) testConvolution.c ./../../lib/convolution/convolution.c $(NEONLIBS) -o testNEON.o

source:
	$(CC) -O3 -Wa,-adhln $(CFLAGS) testConvolution.c ./../../lib/convolution/convolution.c $(LIBS) -o test.o > estudio.s

sourceNEON:
	$(CC) -O3 -Wa,-adhln $(CFLAGS) testConvolution.c ./../../lib/convolution/convolution.c $(NEONLIBS) -o testNEON.o > estudio.s

clean:
	$(RM) *.o *.s *~
