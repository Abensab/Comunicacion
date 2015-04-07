#ifndef SUPERWAV
#define SUPERWAV

    typedef struct SuperWAVTag{
        int leido1;
        int leido2;
        unsigned char **filewav;

    } SuperWAV;

    typedef struct FileHandelTag{
        FILE *fp;
        char dd[100];
    } FileHandel;

    void *buffers(char * archivos_senal);
    int kbhit(void);
    snd_pcm_t * assignPCMName( snd_pcm_t *playback_handle, snd_pcm_stream_t stream, int err);
    snd_pcm_t * configurePlayBack_handle(snd_pcm_t *playback_handle,int err);
    char * handleWAVFiles();
    SuperWAV loadFile();
    void writeFile(FileHandel file,unsigned char **filewav, int l1);

#endif