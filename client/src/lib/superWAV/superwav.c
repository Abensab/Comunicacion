#include "superwav.h"

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


//no está en el .h
void printBufferValues(int** Buff, int bufferSize, int chanelsNumber){
    int i, j;
    for (i = 0; i < bufferSize; ++i) {
        for(j = 0; j < chanelsNumber; j++){
            printf("channel: %d\t\t value: %d\t\t", j, Buff[j][i]);
        }
        printf("\n");
    }
}

/**********************************************************************/
/* START PLAY */
/**********************************************************************/
int superWav(Player *playerArguments){

    /* This holds the error code returned */
    int err = 0;

    /* Handle for the PCM device */
    snd_pcm_t *playback_handle = NULL;

    /* Playback stream */
    snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;

    playback_handle = assignPCMName(playback_handle,stream, (char*) playerArguments->card.pcm_name,err);

    playback_handle = configurePlayBack_handle(playback_handle,playerArguments->card,err);

    // Indice general
    int j;

    // =================================================================================================================
    // Buffer temporal de cada pista donde temporalBuffGnerado[pista] contiene la pista
    int* temporalBuffGnerado[playerArguments->speakers.chanels_number];
    for (j = 0; j < playerArguments->speakers.chanels_number; ++j) {
        temporalBuffGnerado[j] = NULL;
    }

    // ===================================================
    // Buffer con la suma de todas las pistas
    int** buffTotal;
    buffTotal= (int **) malloc ( playerArguments->speakers.chanels_number * sizeof(int *));
    for (j = 0; j < playerArguments->speakers.chanels_number; ++j) {
        buffTotal[j] = NULL;
    }

    // =================================================================================================================

    float filter[64] ={-0.00661109583724264,-0.00650043529566125,-0.00632930310706349,-0.00617482490003971,-0.00602953993132536,-0.00598452022424796,-0.00604361199320415,-0.00630563700873370,-0.00677794258748761,-0.00755797763617839,-0.00864652310346542,-0.0101290944464563,-0.0119893471647713,-0.0142908353440042,-0.0169911140198601,-0.0201243563895989,-0.0236165930309936,-0.0274695564585432,-0.0315773165621993,-0.0359115018748141,-0.0403393430824099,-0.0448101568406542,-0.0491745430350945,-0.0533717619323007,-0.0572496004201327,-0.0607520798261281,-0.0637393605766290,-0.0661751281673016,-0.0679458772409232,-0.0690473584402903,2.29712671743954,-0.0690473584402903,-0.0679458772409232,-0.0661751281673016,-0.0637393605766290,-0.0607520798261281,-0.0572496004201327,-0.0533717619323007,-0.0491745430350945,-0.0448101568406542,-0.0403393430824099,-0.0359115018748141,-0.0315773165621993,-0.0274695564585432,-0.0236165930309936,-0.0201243563895989,-0.0169911140198601,-0.0142908353440042,-0.0119893471647713,-0.0101290944464563,-0.00864652310346542,-0.00755797763617839,-0.00677794258748761,-0.00630563700873370,-0.00604361199320415,-0.00598452022424796,-0.00602953993132536,-0.00617482490003971,-0.00632930310706349,-0.00650043529566125,-0.00661109583724264,0,0,0};
    int lengthFilter = 64;

    // Buffers temporal con el residuo de la Convolución para cada pista
    float** buffer_process_convolution;
    int size_buffer_process_convolution = nextpw2(playerArguments->card.buffer+lengthFilter-1);
    buffer_process_convolution= (float **) malloc ( playerArguments->speakers.chanels_number * sizeof(float *));
    for (j = 0; j < playerArguments->speakers.chanels_number; ++j) {
        buffer_process_convolution[j] = (float *) calloc(size_buffer_process_convolution, sizeof(float));;
    }

    // =================================================================================================================

    // Buffer donde se cópia los valores de A indiferente de si es o no potencia de 2
    // En caso de no tener más valores se queda en 0
    float * processBuffer = (float *) malloc(playerArguments->card.buffer * sizeof(float));


    // =================================================================================================================

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
    int max_Lenth_Song = (maxLenghFile+maxDellay)/512;
    while(playerArguments->l1 < max_Lenth_Song ){

        /*2048bits /4 bits/byte = 512bytes*/
        /*Para avanzar 512 byts necesarios en el buffs*/

        setBuffMem(buffTotal, playerArguments->card.buffer, playerArguments->speakers.chanels_number);

        pthread_mutex_lock(&playerArguments->lock);

        int soundIndex;
        // Recorrer cada audio para aplicarle el WFS y almacenadrlo en el buffTotalFinal
        for (soundIndex = 0; soundIndex < playerArguments->sound.sounds_number; soundIndex++) {
            // Aplicar amplitud y retardo
            generateSongWFS(temporalBuffGnerado, playerArguments->l1, playerArguments->fileWAV, soundIndex, playerArguments->card.buffer,
                            playerArguments->wfsVector[soundIndex], playerArguments->speakers.chanels_number);
            // Sumar el resultando al buffer resultante.
            addToBuffer(buffTotal, temporalBuffGnerado, playerArguments->card.buffer, playerArguments->speakers.chanels_number);
        }

        pthread_mutex_unlock(&playerArguments->lock);

        // Realiza la convolución resultante en cada canal. De esa manera no se realiza tanto cómputo
        for(j = 0; j < playerArguments->speakers.chanels_number; j++) {
            // Make convolution
            //set memory to 0
            memset(processBuffer, 0, playerArguments->card.buffer * sizeof(float));
            int z;
            //int pos = playerArguments->l1 * playerArguments->card.buffer;
            // copiar A a vector de procesamiento
            for (z = 0; z < playerArguments->card.buffer; ++z) {
                processBuffer[z] = (float)buffTotal[j][z];
            }
            //Convolución
            convolutionOverlapAdd8INT(processBuffer, playerArguments->card.buffer, buffer_process_convolution[j], size_buffer_process_convolution, filter, lengthFilter, buffTotal[j]);
        }

        //Reproducción del sonido
        /************************/
        pthread_mutex_lock(&playerArguments->lock);
        while ((pcmreturn = snd_pcm_writen(playback_handle, castBufferToVoid(buffTotal,playerArguments->speakers.chanels_number), playerArguments->card.buffer)) < 0) {
            fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
            break;
        }
        /************************/
        pthread_mutex_unlock(&playerArguments->lock);

        playerArguments->l1++;
        maxDellay = ceil(getMaxFloatVector(playerArguments->wfsVector, playerArguments->sound.sounds_number, playerArguments->speakers.speakers_number));

        if(playerArguments->finishPlaying == TRUE){
            break;
        }
        // Resetea la memoria del bufferTotal
        setBuffMem(buffTotal, playerArguments->card.buffer, playerArguments->speakers.chanels_number);

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
