
// Mi prueba de AUDIO --- 

#include <stdio.h>   
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>                // for gettimeofday()
#include <alsa/asoundlib.h>
#include "spatiallib.h"
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h> /*for true and false*/
#include "superwavlib.h"
#include "connection.h"

#define BUFF_SIZE 4096
#define WORD_LENGTH 50


int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

snd_pcm_t * assignPCMName( snd_pcm_t *playback_handle, snd_pcm_stream_t stream, int err){
    /* Name of the PCM device, like plughw:0,0          */
    /* The first number is the number of the soundcard, */
    /* the second number is the number of the device.   */
    char *pcm_name;

    /* Init pcm_name. Of course, later you */
    /* will make this configurable ;-)     */
    pcm_name = strdup("plughw:1,0");

    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    // -- snd_pcm_hw_params_alloca(&hwparams);

    /* Open PCM. The last parameter of this function is the mode. */
    /* If this is set to 0, the standard mode is used. Possible   */
    /* other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.       */
    /* If SND_PCM_NONBLOCK is used, read / write access to the    */
    /* PCM device will return immediately. If SND_PCM_ASYNC is    */
    /* specified, SIGIO will be emitted whenever a period has     */
    /* been completely processed by the soundcard.                */
    /* Open PCM. The last parameter of this function is the mode. */
    if ((err = snd_pcm_open (&playback_handle, pcm_name, stream, 0)) < 0) {
        fprintf(stderr, "cannot open audio device default for Jetson %s (%s)\n", pcm_name, snd_strerror(err));

        fprintf (stdout, "Testing the default device \n");
        pcm_name = strdup("plughw:0,0");

        if ((err = snd_pcm_open (&playback_handle, pcm_name, stream, 0)) < 0) {
            fprintf(stderr, "cannot open audio device default for PC %s (%s)\n", pcm_name, snd_strerror(err));
            exit(1);
        }
    }
    printf("pcm_name = %s\n", pcm_name);

    return playback_handle;
}

snd_pcm_t * configurePlayBack_handle(snd_pcm_t *playback_handle,int err){
    int rate = 44100; /* Sample rate */
    unsigned int exact_rate; /* Sample rate returned by */

    /* This structure contains information about the hardware and can be used to specify the configuration to be used for */
    /* the PCM stream. */
    snd_pcm_hw_params_t *hw_params;

    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
        fprintf (stderr, "cannot allocate hardware parameters (%s)\n", snd_strerror (err));
        exit (1);
    }

    /* Init hwparams with full configuration space */
    if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0) {
        fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n", snd_strerror (err));
        exit (1);
    }


    /* Set access type. */
    if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_NONINTERLEAVED )) < 0) {
        fprintf (stderr, "cannot set access type (%s)\n", snd_strerror (err));
        exit (1);
    }

    /*AQUI LE DECIMOS QUE UNA MUESTRA DE AUDIO ESTA FORMADA POR 32 BITS :   4 BYTES.
    * Y POR TANTO 1 MUESTRA OCUPA 4 BYTES, o equivale 4 unsigned chars.
    */

    /* Set sample format */
    if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S32_LE)) < 0) {
        fprintf (stderr, "cannot set sample format (%s)\n", snd_strerror (err));
        exit (1);
    }

    /* Set sample rate. If the exact rate is not supported by the hardware, use nearest possible rate. */
    exact_rate = rate;
    if ((err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &exact_rate, 0)) < 0) {
        fprintf (stderr, "cannot set sample rate (%s)\n", snd_strerror (err));
        exit (1);
    }

    if (rate != exact_rate) {
        fprintf(stderr, "The rate %d Hz is not supported by your hardware.\n ==> Using %d Hz instead.\n", rate, exact_rate);
    }

    /* Set number of channels */
    if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, 2)) < 0) {
        fprintf (stderr, "cannot set channel count (%s)\n", snd_strerror (err));
        exit (1);
    }

    /* Apply HW parameter settings to PCM device and prepare device. */
    if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0) {
        fprintf (stderr, "cannot set parameters (%s)\n", snd_strerror (err));
        exit (1);
    }

    /* These values are pretty small, might be useful in
      situations where latency is a dirty word. */
    snd_pcm_uframes_t buffer_size = 2048;
    snd_pcm_uframes_t period_size = 256;

    snd_pcm_hw_params_set_buffer_size_near (playback_handle, hw_params, &buffer_size);
    snd_pcm_hw_params_set_period_size_near (playback_handle, hw_params, &period_size, NULL);
    printf("  ---- buffer_size: %lu\n", buffer_size);
    printf("  ---- period_size: %lu\n", period_size);

    printf("  ---- -------------------------------------------------\n");


    snd_pcm_uframes_t p_size;
    snd_pcm_uframes_t t_size;
    int dir=0;
    snd_pcm_hw_params_get_buffer_size(hw_params, &p_size);
    snd_pcm_hw_params_get_period_size(hw_params, &t_size, &dir);

    printf("  Returned : Buffer size in frames: %lu\n", p_size);
    printf("	Returned approximate period size in frames: %lu\n", t_size);
    printf("	dir: %d\n", dir);

    snd_pcm_hw_params_get_buffer_size_min(hw_params, &p_size);
    printf("  Returned : Returned approximate minimum buffer size in frames: %lu\n", p_size);

    snd_pcm_hw_params_get_buffer_size_max(hw_params, &p_size);
    printf("  Returned : Returned approximate maximum buffer size in frames: %lu\n", p_size);

    snd_pcm_hw_params_free (hw_params);

    if ((err = snd_pcm_prepare (playback_handle)) < 0) {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n", snd_strerror (err));
        exit (1);
    }

    return playback_handle;
}

char * handleWAVFiles(){
    // -------------------------------------- HANDLE OF WAV FILES -------------------------------------//
    int muestrasleidas = 0;
    char * archivos_senal;

    /*Modificado la ruta de ./FicherosPrueba/001_piano.wav
    * para que lea desde una carpeta inferior
    * */
    archivos_senal= (char *)calloc(4* WORD_LENGTH, sizeof(char));
    strcpy(archivos_senal + 0*WORD_LENGTH, "./bin/sound/001_piano.wav");
    strcpy(archivos_senal + 1*WORD_LENGTH, "./bin/sound/voz4408.wav");
    strcpy(archivos_senal + 2*WORD_LENGTH, "./bin/sound/001_bajo.wav");
    strcpy(archivos_senal + 3*WORD_LENGTH, "./bin/sound/001_bateriabuena.wav");

// -------------------------------------- HANDLE OF ALSA DEVICE -------------------------------------//

    return archivos_senal;
}

SuperWAV loadFile(){
    SuperWAV filewav;

    filewav.filewav = (unsigned char **) malloc (2*sizeof(unsigned char *));

    char * archivos_senal = handleWAVFiles();

    filewav.leido1 = OpenWavConvert32(&filewav.filewav[0],archivos_senal);
    printf("leidos 1 =%d\n", filewav.leido1);
    filewav.leido2 = OpenWavConvert32(&filewav.filewav[1],archivos_senal + 1*WORD_LENGTH);
    printf("leidos 2 =%d\n", filewav.leido2);
    return filewav;
}



FileHandel createWriteFile(){
    /*Imprimir o guardar en un fichero los tiempos*/
    /**********************************************/
    FileHandel file;

    file.fp = fopen("./Guarda_Muestras_Y_Tiempo.txt", "w" );
    if (file.fp==NULL)
    {
        printf("Error al abrir el archivo \n");
        exit(1);
    }

    /**********************************************/
    return file;
}

void writeFile(FileHandel file,unsigned char **filewav, int l1){
    /*Imprimir o guardar en un fichero los tiempos*/
    /**********************************************/

    int* aux1= (int*)(filewav[0] + l1*2048);
    int* aux2= (int*)(filewav[1] + l1*2048);
    int cont;
    for(cont = 0; cont < 2048; cont++) {
        //Imprimir por pantalla
        //printf("\tA\t%d\t%d\t%lld\n",cont,*(aux1 + cont),current_timestamp());
        //printf("\tB\t%d\t%d\t%lld\n",cont,*(aux2 + cont),current_timestamp());

        //Escribir en fichero
        sprintf(file.dd,"\t%lld\t%d\t%d\n",current_timestamp(),*(aux1 + cont),*(aux2 + cont));
        fputs(file.dd,file.fp);
    }
    /**********************************************/
}


int main (int argc, char *argv[])
{
    /* This holds the error code returned */
    int err;
    short buf[BUFF_SIZE];

    SuperWAV fileWAV = loadFile();

    /* Handle for the PCM device */
    snd_pcm_t *playback_handle;

    /* Playback stream */
    snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;

    playback_handle = assignPCMName(playback_handle,stream,err);

    playback_handle = configurePlayBack_handle(playback_handle,err);

    /*Declarar los buffers con datos*/
    void *bufs[2] = { NULL , NULL };		// Allocate two (bufs[0],bufs[1]) but we only user lowest one for now
    bufs[0]=(void *)fileWAV.filewav[0];							  // Set the pointer array element zero to pointer bufptr ie **data
    bufs[1]=(void *)fileWAV.filewav[1];							// Set the pointer array element zero to pointer bufptr ie **data
    /*============================================*/

    /*Declarar vector de 0 para tiempo en silencio*/
    void *bufsVoid[2] = { NULL , NULL };
    int voidVector[512];
    memset(voidVector,0,512*sizeof(int));
    bufsVoid[0]=(void *)voidVector;
    bufsVoid[1]=(void *)voidVector;
    /*============================================*/

    // -- FOR REPRODUCCING ---- //
    int pcmreturn;
    int l1;
    printf("size of buf = %lud\n", sizeof(buf));
    printf("size of data = %lud\n", sizeof(buf));

    struct timeval t1, t2;
    double elapsedTime;

    // start timer
    gettimeofday(&t1, NULL);

    /*Imprimir o guardar en un fichero los tiempos*/
    /**********************************************/
    /*
    FileHandel file = createWriteFile();
    */
    /**********************************************/

    bool flag = true;

    for(l1 = 0; l1 < 10000; l1++) {
        if(kbhit()) {
            char c=getchar();
            printf("has presionado %c\n",c);
        }

        /*2048bits /4 bits/byte = 512bytes*/
        /*Para avanzar 512 byts necesarios en el buffs*/
        bufs[0]=(void *)(fileWAV.filewav[0] + l1*2048);							  // Set the pointer array element zero to pointer bufptr ie **data
        bufs[1]=(void *)(fileWAV.filewav[1] + l1*2048);							  // Set the pointer array element zero to pointer bufptr ie **data

        /*Imprimir o guardar en un fichero los tiempos*/
        /**********************************************/
        /*
        writeFile(file,fileWAV.filewav, l1);
        */
        /**********************************************/

        /*Comprobar si le toca reproducir por el flag.*/

        if(flag == true){
            //Reproducción del sonido
            /************************/
            while ((pcmreturn = snd_pcm_writen(playback_handle, bufs, 512)) < 0){
                printf("HOLA HOLA HOLA HOLA HOLA HOLA\n");
                // snd_pcm_prepare(playback_handle);
                fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
                break;
            }
            /************************/
        }else{

            //Reproducción de zeros
            /************************/
            while ((pcmreturn = snd_pcm_writen(playback_handle, bufsVoid, 512)) < 0){
                printf("HOLA HOLA HOLA HOLA HOLA HOLA\n");
                // snd_pcm_prepare(playback_handle);
                fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
                break;
            }
            /************************/
        }

        if(l1 == 1000){
            flag = false;
        }

        if(l1 == 1500){
            flag = true;
        }
    }

    /*Imprimir o guardar en un fichero los tiempos*/
    /**********************************************/
    /*
    fflush(file.fp);
    fclose(file.fp);
    */
    /**********************************************/

    // stop timer
    gettimeofday(&t2, NULL);

    // compute and print the elapsed time in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms

    printf(" -- PASADO %f -- \n", elapsedTime);

    freeWav(fileWAV.filewav[0]);
    freeWav(fileWAV.filewav[1]);

    snd_pcm_close (playback_handle);

    return 0;
}