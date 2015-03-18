
// Mi prueba de AUDIO --- 

#include <stdio.h>   
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>                // for gettimeofday()
#include <alsa/asoundlib.h>


#define POLY 10
#define GAIN 5000.0
#define BUFF_SIZE 4096
#define WORD_LENGTH 50

#include "spatiallib.h"


#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
 
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


int main (int argc, char *argv[]) 
{

  int err;
  int tt=0;
  short buf[BUFF_SIZE];
  int rate = 44100; /* Sample rate */
  unsigned int exact_rate; /* Sample rate returned by */
  // -------------------------------------- HANDLE OF WAV FILES -------------------------------------//
    // int ifor = 0;
  int muestrasleidas = 0;
  char * archivos_senal;
    
    /*Modificado la ruta de ./FicherosPrueba/001_piano.wav
    * para que lea desde una carpeta inferior
    * */
  // filewav32 = (unsigned char **) malloc (4*sizeof(unsigned char *));
	archivos_senal= (char *)calloc(4* WORD_LENGTH, sizeof(char));
	strcpy(archivos_senal + 0*WORD_LENGTH, "./bin/FicherosPrueba/001_piano.wav");
  strcpy(archivos_senal + 1*WORD_LENGTH, "./bin/FicherosPrueba/voz4408.wav");
 	strcpy(archivos_senal + 2*WORD_LENGTH, "./bin/FicherosPrueba/001_bajo.wav");
	strcpy(archivos_senal + 3*WORD_LENGTH, "./bin/FicherosPrueba/001_bateriabuena.wav");

	// for(ifor=0; ifor<4; ifor++)
    //     muestrasleidas = readmewav(ifor, archivos_senal + ifor*WORD_LENGTH);   //cantidad de datos de entrada

    // -- cogemos muestras de char en char.


// -------------------------------------- HANDLE OF ALSA DEVICE -------------------------------------//
  
   
  /* Handle for the PCM device */
  snd_pcm_t *playback_handle;
  /* Playback stream */
  snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
  /* This structure contains information about the hardware and can be used to specify the configuration to be used for */
  /* the PCM stream. */
  snd_pcm_hw_params_t *hw_params;

    
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
    fprintf(stderr, "cannot open audio device default for Jetson (%s)\n", snd_strerror(err));

    fprintf (stdout, "Testing the default device \n");
    pcm_name = strdup("plughw:0,0");

    if ((err = snd_pcm_open (&playback_handle, pcm_name, stream, 0)) < 0) {
        fprintf(stderr, "cannot open audio device default for PC (%s)\n", snd_strerror(err));
        exit(1);
    }
}
    printf("pcm_name = %s\n", pcm_name);
    
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


// printf("   1 frame tiene %lu bytes\n",sizeof(p_size));
 
 
snd_pcm_hw_params_free (hw_params);
 
if ((err = snd_pcm_prepare (playback_handle)) < 0) {
    fprintf (stderr, "cannot prepare audio interface for use (%s)\n", snd_strerror (err));
    exit (1);
}



     // unsigned char * file16rep = newWav2chNonIntervaled(archivos_senal, archivos_senal + 1*WORD_LENGTH);
     // unsigned char * file16rep = newWav2ch(archivos_senal, archivos_senal + 1*WORD_LENGTH);
     int leido1=0;
     int leido2=0;
     
     unsigned char **penter = (unsigned char **) malloc (2*sizeof(unsigned char *));
     unsigned char **filewav = (unsigned char **) malloc (2*sizeof(unsigned char *));
     leido1 = OpenWavConvert32(&filewav[0],archivos_senal);
     printf("leidos 1 =%d\n", leido1);
     leido2 = OpenWavConvert32(&filewav[1],archivos_senal + 1*WORD_LENGTH);
     printf("leidos 2 =%d\n", leido2);
     // penter[0] = filewav[0];
     // penter[1] = filewav[1];	
     
     
     void *bufs[2] = { NULL , NULL };		// Allocate two (bufs[0],bufs[1]) but we only user lowest one for now  
     bufs[0]=(void *)filewav[0];							  // Set the pointer array element zero to pointer bufptr ie **data
     bufs[1]=(void *)filewav[1];							// Set the pointer array element zero to pointer bufptr ie **data


    // bufs[0]=(void *)data;							  // Set the pointer array element zero to pointer bufptr ie **data
    // bufs[1]=(void *)data2;							// Set the pointer array element zero to pointer bufptr ie **data

     printf("AQUI NO LLEGAMOS\n");

/*      int pcmreturn, l1, l2;
      short s1, s2;

      for(l2 = 0; l2 < 1024; l2++) {
        s1 = (l2 % 128) * 100 - 5000;  
        s2 = (l2 % 256) * 100 - 5000;  
        buf[4*l2] = (unsigned char)s1;
        buf[4*l2+1] = s1 >> 8;
        buf[4*l2+2] = (unsigned char)s2;
        buf[4*l2+3] = s2 >> 8;
      }

*/


    // -- FOR REPRODUCCING ---- //
    unsigned char *data;
    int pcmreturn, l1, l2;
    short s1, s2;
    int frames;
    snd_pcm_uframes_t periodsize = BUFF_SIZE*sizeof(short);
    
    printf("size of buf = %lud\n", sizeof(buf));
    
    // printf("periodisize = %lu\n", periodsize);
    
    // printf("HEMOS PASADO_ TOTAL\n");

    data = (unsigned char *)malloc(periodsize);
    
    printf("size of data = %lud\n", sizeof(buf));
    
    frames = periodsize >> 2;
    
    struct timeval t1, t2;
    double elapsedTime;
    
    
    // start timer
    gettimeofday(&t1, NULL);
    
    int pasamos = 0;    
    for(l1 = 0; l1 < 10000; l1++) {
/*      for(l2 = 0; l2 < 5000; l2++) {
        s1 = (l2 % 128) * 100 - 5000;  
        s2 = (l2 % 256) * 100 - 5000;  
        data[4*l2] = (unsigned char)s1;
        data[4*l2+1] = s1 >> 8;
        data[4*l2+2] = (unsigned char)s2;
        data[4*l2+3] = s2 >> 8;
      }*/
      // printf("VAMOS PARA ALLA\n");
      // while ((pcmreturn = snd_pcm_writen(playback_handle, (void **)data, 4096)) < 0){
      
     if(kbhit())
     {
       char c=getchar();
       printf("has presionado %c\n",c);
     }



      bufs[0]=(void *)(filewav[0] + l1*2048);							  // Set the pointer array element zero to pointer bufptr ie **data
      bufs[1]=(void *)(filewav[1] + l1*2048);							  // Set the pointer array element zero to pointer bufptr ie **data

      //----- EL BUENO SON 2048
      while ((pcmreturn = snd_pcm_writen(playback_handle, bufs, 512)) < 0){
        printf("HOLA HOLA HOLA HOLA HOLA HOLA\n");
        // snd_pcm_prepare(playback_handle);
        fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
        break;
      }
      //pasamos ++;
      //sleep(1);
      // printf("hola --\n");
      // printf("writing: %d\n", pcmreturn);
    }
    
    
    
    // stop timer
    gettimeofday(&t2, NULL);

    // compute and print the elapsed time in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    
    printf(" -- PASADO %f -- \n", elapsedTime);

    

    freeWav(filewav[0]);
    freeWav(filewav[1]);

      /* Write some junk data to produce sound. */
/*  for(tt=0; tt< 150; tt++)
  {
      if ((err = snd_pcm_writei (playback_handle, buf, BUFF_SIZE/2)) != BUFF_SIZE/2) {
          fprintf (stderr, "write to audio interface failed (%s)\n", snd_strerror (err));
          exit (1);
      } else {
          fprintf (stdout, "snd_pcm_writei successful\n");
      }
  } */
  



snd_pcm_close (playback_handle);




    
    // -- FOR REPRODUCCING ---- //
 /*   unsigned char *data;
    int pcmreturn, l1, l2;
    short s1, s2;
    int frames;

    printf("HEMOS PASADO_ TOTAL\n");

    data = (unsigned char *)malloc(periodsize);
    frames = periodsize >> 2;
    for(l1 = 0; l1 < 100; l1++) {
      for(l2 = 0; l2 < num_frames; l2++) {
        s1 = (l2 % 128) * 100 - 5000;  
        s2 = (l2 % 256) * 100 - 5000;  
        data[4*l2] = (unsigned char)s1;
        data[4*l2+1] = s1 >> 8;
        data[4*l2+2] = (unsigned char)s2;
        data[4*l2+3] = s2 >> 8;
      }
      while ((pcmreturn = snd_pcm_writei(pcm_handle, (void **)data, frames)) < 0) {
        snd_pcm_prepare(pcm_handle);
        fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
      }
    }
  
    
    /* Stop PCM device after pending frames have been played  
    snd_pcm_drain(pcm_handle);*/
    
    
    
    // - snd_pcm_close(pcm_handle);
    // - snd_pcm_close (pcm_handle);
    // - snd_seq_close (seq_handle);
    
    return 0;

}
