#ifndef SUPERWAV
#define SUPERWAV

    typedef struct SuperWAVTag{
        int leido1;
        int leido2;
        unsigned char **filewav;

    } SuperWAV;

     void *buffers(char * archivos_senal);

#endif