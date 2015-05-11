#include "./../../include/client.h"
/*  A simple server in the internet domain using TCP
    The port number is passed as an argument
    http://www.linuxhowtos.org/C_C++/socket.htm
*/

#define WORD_LENGTH 50


ClientConnection startConfigurationClient(char *address, int portNumber){

    ClientConnection client;

    client.socketFileDescriptor = Socket(AF_INET, SOCK_STREAM, 0);
    //fcntl(client.socketFileDescriptor, F_SETFL, O_NONBLOCK);

    client.server = GetHostByName(address);

    bzero((char *) &client.server_addr, sizeof(client.server_addr));
    client.server_addr.sin_family = AF_INET;
    bcopy((char *) client.server->h_addr, (char *) &client.server_addr.sin_addr.s_addr, client.server->h_length);
    client.server_addr.sin_port = htons(portNumber);

    Connect(client.socketFileDescriptor, (struct sockaddr *) &client.server_addr, sizeof(client.server_addr));

    return client;
}

char **strsplit(const char* str, const char* delim, size_t* numtokens) {
    // copy the original string so that we don't overwrite parts of it
    // (don't do this if you don't need to keep the old line,
    // as this is less efficient)
    char *s = strdup(str);

    // these three variables are part of a very common idiom to
    // implement a dynamically-growing array
    size_t tokens_alloc = 1;
    size_t tokens_used = 0;
    char **tokens = calloc(tokens_alloc, sizeof(char*));

    char *token, *strtok_ctx;
    for (token = strtok_r(s, delim, &strtok_ctx);
         token != NULL;
         token = strtok_r(NULL, delim, &strtok_ctx)) {
        // check if we need to allocate more space for tokens
        if (tokens_used == tokens_alloc) {
            tokens_alloc *= 2;
            tokens = realloc(tokens, tokens_alloc * sizeof(char*));
        }
        tokens[tokens_used++] = strdup(token);
    }
    // cleanup
    if (tokens_used == 0) {
        free(tokens);
        tokens = NULL;
    } else {
        tokens = realloc(tokens, tokens_used * sizeof(char*));
    }
    *numtokens = tokens_used;
    free(s);

    return tokens;
}

Player setArguments(char *str){

    Player argumets;
    printf("%s\n",str);

    argumets.bufs = (void **) malloc (2*sizeof(void *));
    argumets.l = 0;

    char **variableAndValue;
    size_t numvariableAndValue;

    variableAndValue = strsplit(str, ",", &numvariableAndValue);

    size_t i;
    for (i = 0; i < numvariableAndValue; i++) {
        //printf("Variable And Value: \"%s\"\n", variableAndValue[i]);

        char **variableORValue;
        size_t twoValues;
        variableORValue = strsplit(variableAndValue[i], ":", &twoValues);

        if(i == 0){
            argumets.timeToStart = atoll(variableORValue[1]);
        }

        if(i == 1){
            argumets.IDPlaying= atoi(variableORValue[1]);
        }

        free(variableORValue[0]);
        free(variableORValue[1]);
        free(variableAndValue[i]);
    }

    return argumets;
}

char * handleWAVFiles(){
    // -------------------------------------- HANDLE OF WAV FILES -------------------------------------//
    char * archivos_senal;

    /*Modificado la ruta de ./FicherosPrueba/001_piano.wav
    * para que lea desde una carpeta inferior
    * */
    archivos_senal= (char *)calloc(4* WORD_LENGTH, sizeof(char));
    strcpy(archivos_senal + 0*WORD_LENGTH, "./../bin/sound/001_piano.wav");
    strcpy(archivos_senal + 1*WORD_LENGTH, "./../bin/sound/voz4408.wav");
    strcpy(archivos_senal + 2*WORD_LENGTH, "./../bin/sound/001_bajo.wav");
    strcpy(archivos_senal + 3*WORD_LENGTH, "./../bin/sound/001_bateriabuena.wav");

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

int startClientConnection(char *address, int portNumber){
    int                 bytes;
    char                buffer[256];
    ClientConnection    client  = startConfigurationClient(address, portNumber);

    pthread_t           playerThread;

    Player arguments;

    /*ID*/
    bzero(buffer,256);
    bytes = Recv(client.socketFileDescriptor,buffer,256,0);

    client.clientID = atoi(buffer);
    printf("My ID: %d \n", client.clientID);

    /* timestamp, funete, posIni, velocidad, posFin, idCli*/
    /*inicialmente: timestamp, idCli */
    bzero(buffer,256);
    bytes = Recv(client.socketFileDescriptor,buffer,256,0);

    printf("Milliseconds message recived: %lld\n", current_timestamp());

    arguments = setArguments(buffer);

    /**/
    SuperWAV fileWAV = loadFile();
    arguments.bufs[0]=(void *)fileWAV.filewav[0];             // Set the pointer array element zero to pointer bufptr ie **data
    arguments.bufs[1]=(void *)fileWAV.filewav[1];             // Set the pointer array element zero to pointer bufptr ie **data
    /**/


    /*Declarar vector de 0 para tiempo en silencio*/
    void *bufsVoid[2] = { NULL , NULL };
    int voidVector[512];
    memset(voidVector,0,512*sizeof(int));
    bufsVoid[0]=(void *)voidVector;
    bufsVoid[1]=(void *)voidVector;
    /*============================================*/






    if(arguments.IDPlaying == client.clientID){
        arguments.flag = TRUE;
    }else{
        arguments.flag = FALSE;
    }
    /*Started to play*/
    arguments.finishPlaying = FALSE;

    // Make sure it can be shared across processes
    pthread_mutexattr_t shared;
    pthread_mutexattr_init(&shared);
    pthread_mutexattr_setpshared(&shared, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&arguments.lock, &shared);

    long long delay = (arguments.timeToStart-10000) - current_timestamp();
    printf("Delay (10s program): %lld\n",delay);

    /*int x = 0;*/
    /* create a second thread which executes inc_x(&x) */
    if(pthread_create(&playerThread, NULL, playSuperWav, &arguments)) {
        fprintf(stderr, "Error creating player thread\n");
        return 1;
    }

    Player newArguments;

    for(;;) {
        bzero(buffer, 256);
        bytes = Recv(client.socketFileDescriptor, buffer, 255, 0);

        newArguments = setArguments(buffer);

        if(bytes>0){

            printf("New ID Client PLayer: %d, Client: %d\n", newArguments.IDPlaying, client.clientID);
            fflush(stdout);
            if(newArguments.IDPlaying == client.clientID){
                pthread_mutex_lock(&arguments.lock);
                arguments.flag = TRUE;
                //printf("Son iguales\n");
                fflush(stdout);
                pthread_mutex_unlock(&arguments.lock);
            }else{
                pthread_mutex_lock(&arguments.lock);
                arguments.flag = FALSE;
                //printf("NO son iguales\n");
                fflush(stdout);
                pthread_mutex_unlock(&arguments.lock);
            }
            if(newArguments.IDPlaying == -1){
                printf("\n¡¡Se acabo!!\n");
                fflush(stdout);
                arguments.flag = -1;
                break;
            }
        }
        if(arguments.finishPlaying == TRUE){
            break;
        }
    }
    pthread_mutex_destroy(&arguments.lock);
    pthread_exit(NULL);
    close(client.socketFileDescriptor);

    return 0;
}