CC = gcc
CFLAGS  = -g -Wall -Wextra

LIBS = -lasound -lm -lpthread -lconfig

SRCS = lib/functions/functions.c lib/parsher/parsher.c lib/configuration/configuration.c lib/socketUtils/socketUtils.c lib/client/client.c lib/spacial/spatial.c lib/processSount/processSound.c lib/superWAV/superwav.c

all:
	$(CC) SuperWavAppClient.c $(SRCS)  -o SuperWAVAppClient.o $(LIBS)

debug:
	$(CC) $(CFLAGS) SuperWavAppClient.c $(SRCS) -o SuperWAVAppClient_Debug.o $(LIBS)

clean:
	$(RM) *.o *~
