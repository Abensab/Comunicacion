CC = gcc
CFLAGS  = -g -Wall -Wextra

LIBS = ../../../lib/fft/fftGS.c -lm -lfftw3f -L./../../../../../bin/fftw/fftw-3.3.4/ -lfftw3
FFTW = ../../../../../bin/fftw/fftw-3.3.4/.libs/libfftw3f.a 

NEONLIBS = -march=armv7 -mfpu=neon

all:
	$(CC) -O3 convolution.c $(LIBS) $(NEONLIBS) -o convolution.o
	$(CC) -O3 convolutionOverlapAdd.c $(LIBS) $(NEONLIBS) -o convolutionOverlapAdd.o
	$(CC) -O3 convolutionOverlapAdd8.c $(LIBS) $(NEONLIBS) -o convolutionOverlapAdd8.o
	$(CC) -O3 convolutionNeon.c $(LIBS) $(NEONLIBS) -o convolutionNeon.o
	$(CC) -O3 convolutionOverlapAddNEON.c $(LIBS) $(NEONLIBS) -o convolutionOverlapAddNEON.o
	$(CC) -O3 convolutionFFT.c $(LIBS) $(NEONLIBS) -o convolutionFFT.o
	$(CC) -O3 convolutionOverlapSave.c $(LIBS) $(NEONLIBS) -o convolutionOverlapSave.o
	$(CC) -O3 convolutionFFTW.c $(LIBS) $(NEONLIBS) -o convolutionFFTW.o
	$(CC) -O3 convolutionOverlapSave.c $(LIBS) $(NEONLIBS) -o convolutionOverlapSaveFFTW.o

comparation:
	$(CC) -O3 comparation.c $(LIBS) $(NEONLIBS) -o comparation.o
conv:
	$(CC) -O3 convolution.c $(LIBS) $(NEONLIBS) -o convolution.o
convADD:
	$(CC) -O3 convolutionOverlapAdd.c $(LIBS) $(NEONLIBS) -o convolutionOverlapAdd.o
convADD8:
	$(CC) -O3 convolutionOverlapAdd8.c $(LIBS) $(NEONLIBS) -o convolutionOverlapAdd8.o
convNeon:
	$(CC) -O3 convolutionNeon.c $(LIBS) $(NEONLIBS) -o convolutionNeon.o
convNeonAdd:
	$(CC) -O3 convolutionOverlapAddNEON.c $(LIBS) $(NEONLIBS) -o convolutionOverlapAddNEON.o
convFFT:
	$(CC) -O3 convolutionFFT.c $(LIBS) $(NEONLIBS) -o convolutionFFT.o
convSave:
	$(CC) -O3 convolutionOverlapSave.c $(LIBS) $(NEONLIBS) -o convolutionOverlapSave.o
convFFTW:
	$(CC) -O3 convolutionFFTW.c $(LIBS) $(FFTW) $(NEONLIBS) -o convolutionFFTW.o
convSaveFFTW:
	$(CC) -O3 convolutionOverlapSaveFFTW.c $(LIBS) $(FFTW) $(NEONLIBS) -o convolutionOverlapSaveFFTW.o


debug:
	$(CC) -O3 $(CFLAGS) comparation.c $(LIBS) $(NEONLIBS) -o comparation.o	

clean:
	$(RM) -r *.o *.o.* *~
