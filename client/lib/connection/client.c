#include "./../../include/client.h"
/*  A simple server in the internet domain using TCP
    The port number is passed as an argument
    http://www.linuxhowtos.org/C_C++/socket.htm
*/

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

    Player playerArgumets;
    printf("%s\n",str);

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
            playerArgumets.timeToStart = atoll(variableORValue[1]);
        }

        if(i == 1){
            playerArgumets.IDPlaying= atoi(variableORValue[1]);
        }

        free(variableORValue[0]);
        free(variableORValue[1]);
        free(variableAndValue[i]);
    }

    return playerArgumets;
}



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


void* playSuperWav(void *arguments) {

    Player *args = (Player *) arguments;
    /*printf("Time to start: %lld\n", args->timeToStart);
    printf("Flag: %d\n", args->flag);
    printf("Finish playing: %d\n", args->finishPlaying);
*/
    int sleepTime = ( (args->timeToStart - current_timestamp())/1000 ) - 1;

    printf("Sleep Time: %d\n",sleepTime);

    while (TRUE){
        sleep(sleepTime);
        if( (args->timeToStart - current_timestamp()) <= 0 ) {
            superWav(args);
            break;
        }/*else esperando*/
    }
    return NULL;
}


/**********************************************************************/
/* START CLIENT */
/**********************************************************************/

int startClientConnection(char *address, int portNumber){
    int                 bytes;
    char                buffer[256];
    ClientConnection    client  = startConfigurationClient(address, portNumber);

    pthread_t           playerThread;

    Player playerArguments;

    //SuperWAV fileWAV = loadFile();

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

    playerArguments = setArguments(buffer);

    //double x = -1.0;
    //double y = 0.0;

    //double **resultWFS = WFS(x,y);

    playerArguments.bufferToPlay = (void **) malloc (2*sizeof(void *));



    if(playerArguments.IDPlaying == client.clientID){
        playerArguments.flag = TRUE;
    }else{
        playerArguments.flag = FALSE;
    }
    /*Started to play*/
    playerArguments.finishPlaying = FALSE;

    // Make sure it can be shared across processes
    pthread_mutexattr_t shared;
    pthread_mutexattr_init(&shared);
    pthread_mutexattr_setpshared(&shared, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&playerArguments.lock, &shared);

    long long delay = (playerArguments.timeToStart-10000) - current_timestamp();
    printf("Delay (10s program): %lld\n",delay);

    /*int x = 0;*/
    /* create a second thread which executes inc_x(&x) */
    if(pthread_create(&playerThread, NULL, playSuperWav, &playerArguments)) {
        fprintf(stderr, "Error creating player thread\n");
        return 1;
    }

    Player newPlayerArguments;

    for(;;) {
        bzero(buffer, 256);
        bytes = Recv(client.socketFileDescriptor, buffer, 255, 0);

        newPlayerArguments = setArguments(buffer);

        if(bytes>0){

            printf("New ID Client PLayer: %d, Client: %d\n", newPlayerArguments.IDPlaying, client.clientID);
            fflush(stdout);
            if(newPlayerArguments.IDPlaying == client.clientID){
                pthread_mutex_lock(&playerArguments.lock);
                playerArguments.flag = TRUE;
                printf("Son iguales\n");
                fflush(stdout);
                pthread_mutex_unlock(&playerArguments.lock);
            }else{
                pthread_mutex_lock(&playerArguments.lock);
                playerArguments.flag = FALSE;
                printf("NO son iguales\n");
                fflush(stdout);
                pthread_mutex_unlock(&playerArguments.lock);
            }
            if(newPlayerArguments.IDPlaying == -1){
                printf("\n¡¡Se acabo!!\n");
                fflush(stdout);
                playerArguments.flag = -1;
                break;
            }
        }
        if(playerArguments.finishPlaying == TRUE){
            break;
        }
    }
    pthread_mutex_destroy(&playerArguments.lock);
    pthread_exit(NULL);
    close(client.socketFileDescriptor);

    return 0;
}
/**********************************************************************/
/* FIN CLIENT */
/**********************************************************************/