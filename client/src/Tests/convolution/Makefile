CC = gcc
CFLAGS  = -Wa,-adhln -g -Wall -Wextra

LIBS = -lm

NEONLIBS = -march=armv7 -mfpu=neon -lm

test:
	$(CC) -O3 testConvolution.c  ./../../lib/convolution/convolution.c $(LIBS) -o test.o

testNEON:
	$(CC) -O3 testConvolution.c ./../../lib/convolution/convolution.c $(NEONLIBS) -o testNEON.o

source:
	$(CC) -O3 $(CFLAGS) testConvolution.c ./../../lib/convolution/convolution.c $(LIBS) -o test.o > estudio.s

sourceNEON:
	$(CC) -O3 $(CFLAGS) testConvolution.c ./../../lib/convolution/convolution.c $(NEONLIBS) -o testNEON.o > estudio.s

clean:
	$(RM) *.o *.s *~
