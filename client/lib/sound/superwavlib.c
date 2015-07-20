#include "../../include/superwavlib.h"

snd_pcm_t * assignPCMName( snd_pcm_t *playback_handle, snd_pcm_stream_t stream, char* pcm_name_config,int err){

    if ((err = snd_pcm_open (&playback_handle, pcm_name_config, stream, 0)) < 0) {
        fprintf(stderr, "cannot open audio device default from configuration file %s (%s)\n", pcm_name_config, snd_strerror(err));
    }else{
        return playback_handle;
    }

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

snd_pcm_t * configurePlayBack_handle(snd_pcm_t *playback_handle, ClientCard cardConfig,int err){
    int rate = cardConfig.frame_Rate; /* Sample rate */
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

    if (rate != (int) exact_rate) {
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
    snd_pcm_uframes_t buffer_size = cardConfig.pcm_buffer_size;
    snd_pcm_uframes_t period_size = cardConfig.pcm_period_size;

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

float getMaxFloatVector(WFS* a, int soundsNumber, int speakersNumber) {
    int c;
    float max, temporalValue;
    max = getMaxFloat(a[0].tn,speakersNumber);
    for (c = 1; c < soundsNumber; c++) {
        temporalValue = getMaxFloat(a[c].tn,speakersNumber);
        if (temporalValue > max) {
            max = temporalValue;
        }
    }
    return max;
}

/**********************************************************************/
/* START PLAY */
/**********************************************************************/
int superWav(Player *playerArguments){

    /* This holds the error code returned */
    int err = 0;

    //SuperWAV fileWAV = loadFile(playerArguments->sound);

    /* Handle for the PCM device */
    snd_pcm_t *playback_handle = NULL;

    /* Playback stream */
    snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;

    playback_handle = assignPCMName(playback_handle,stream, (char*) playerArguments->card.pcm_name,err);

    playback_handle = configurePlayBack_handle(playback_handle,playerArguments->card,err);

    int* pruebaBuffGnerado[playerArguments->speakers.chanels_number];
    int j;
    for (j = 0; j < playerArguments->speakers.chanels_number; ++j) {
        pruebaBuffGnerado[j] = NULL;
    }

    // -- FOR REPRODUCCING ---- //
    int pcmreturn;

    struct timeval t1, t2;
    double elapsedTime;

    // start timer
    gettimeofday(&t1, NULL);

    int maxLenghFile = getMaxInt(playerArguments->fileWAV.leido, playerArguments->sound.sounds_number);
    int maxDellay = ceil(getMaxFloatVector(playerArguments->wfsVector, playerArguments->sound.sounds_number, playerArguments->speakers.speakers_number));

    printf("%d, %d, %d, %d\n",playerArguments->fileWAV.leido[0],playerArguments->fileWAV.leido[1],playerArguments->fileWAV.leido[2],playerArguments->fileWAV.leido[3]);
    printf(" ==> %d\n",maxLenghFile+maxDellay);
    printf(" ==> %d\n",(maxLenghFile+maxDellay)/512);

    //while(playerArguments->l1 < 10000) {
    while(playerArguments->l1 < (maxLenghFile+maxDellay)/512 ){

        /*2048bits /4 bits/byte = 512bytes*/
        /*Para avanzar 512 byts necesarios en el buffs*/

        pthread_mutex_lock(&playerArguments->lock);

        generateSongWFS(pruebaBuffGnerado, playerArguments->l1, playerArguments->fileWAV, 0, playerArguments->card.buffer,
                playerArguments->wfsVector[0], playerArguments->speakers.chanels_number);
        pthread_mutex_unlock(&playerArguments->lock);


        //Reproducción del sonido
        /************************/
        while ((pcmreturn = snd_pcm_writen(playback_handle, castBufferToVoid(pruebaBuffGnerado,playerArguments->speakers.chanels_number), playerArguments->card.buffer)) < 0) {
            printf("HOLA HOLA HOLA HOLA HOLA HOLA\n");
            // snd_pcm_prepare(playback_handle);
            fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
            break;
        }
        /************************/

        pthread_mutex_lock(&playerArguments->lock);
        printf("actual: %d max: %d\n",playerArguments->l1,(maxLenghFile+maxDellay)/512);
        playerArguments->l1++;
        maxDellay = ceil(getMaxFloatVector(playerArguments->wfsVector, playerArguments->sound.sounds_number, playerArguments->speakers.speakers_number));

        pthread_mutex_unlock(&playerArguments->lock);

        if(playerArguments->finishPlaying == TRUE){
            break;
        }

    }

    pthread_mutex_lock(&playerArguments->lock);
    playerArguments->finishPlaying = TRUE;
    pthread_mutex_unlock(&playerArguments->lock);

    // stop timer
    gettimeofday(&t2, NULL);

    // compute and print the elapsed time in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms

    printf(" -- PASADO %f -- \n", elapsedTime);

    snd_pcm_close (playback_handle);

    return 0;
}