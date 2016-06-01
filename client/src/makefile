CC = gcc
CFLAGS  = -g -Wall -Wextra

LIBS = -lasound -lm -lpthread -lconfig

SRCS = lib/functions/functions.c lib/parsher/parsher.c lib/configuration/configuration.c lib/socketUtils/socketUtils.c lib/client/client.c lib/spacial/spatial.c lib/processSount/processSound.c lib/superWAV/superwav.c

NORMAL = lib/convolution/convolution.c

ARM = lib/convolution/convolutionNEON.c
NEONLIBS = -march=armv7 -mfpu=neon

all:
	$(CC) SuperWavAppClient.c $(SRCS)  -o SuperWAVAppClient.o $(LIBS) $(NORMAL)

debug:
	$(CC) $(CFLAGS) SuperWavAppClient.c $(SRCS) -o SuperWAVAppClient_Debug.o $(LIBS) $(NORMAL)

ARM:
	$(CC) $(CFLAGS) SuperWavAppClient.c $(SRCS) -o SuperWAVAppClient_Debug.o $(LIBS) $(ARM) $(NEONLIBS)

clean:
	$(RM) *.o *~
