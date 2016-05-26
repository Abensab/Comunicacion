#ifndef SUPERWAV
#define SUPERWAV

#include "../processSount/processSound.h"
#include <alsa/asoundlib.h>

    typedef struct FileHandelTag{
        FILE *fp;
        char dd[100];
    } FileHandel;

    void *buffers(char * archivos_senal);
    snd_pcm_t * assignPCMName( snd_pcm_t *playback_handle, snd_pcm_stream_t stream, char* pcm_name_config, int err);
    snd_pcm_t * configurePlayBack_handle(snd_pcm_t *playback_handle, ClientCard cardConfig, int err);
    //void writeFile(FileHandel file,unsigned char **filewav, int l1);
    int superWav(Player *playerArguments);

#endif