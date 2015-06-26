#include "./../../include/client.h"
/*  A simple server in the internet domain using TCP
    The port number is passed as an argument
    http://www.linuxhowtos.org/C_C++/socket.htm
*/

int getConfig(Player* playerArguments, char* configFile){

    //Config reader
    config_t            cfg;
    checkConfig(&cfg,configFile);

    playerArguments->card = clientCardGenerator(&cfg);
    playerArguments->sound = clientSoundGenerator(&cfg);
    playerArguments->speakers = clientSpeakersGenerator(&cfg);
    playerArguments->timeToStrartSeconds = timeToStartConfig(&cfg);
    /*Started to play*/
    playerArguments->finishPlaying = FALSE;
    return 0;
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

int startClientConnection(char *address, int portNumber, char* configFile){
    int                 bytes;
    char                buffer[256];
    ClientConnection    client  = startConfigurationClient(address, portNumber);
    pthread_t           playerThread;
    Player              playerArguments;

    MesageVariables     configFromServer;

    //Config reader
    if (strcmp(configFile,"NULL") == 0 ){
        configFile    = "./config/default.cfg";
    }
    getConfig(&playerArguments, configFile);


    // ID
    bzero(buffer,256);
    bytes = Recv(client.socketFileDescriptor,buffer,256,0);
    configFromServer = processMesage(buffer);

    if(bytes<=0){
        return -1;
    }

    if (strcmp(configFromServer.action, "identification") == 0){
        client.clientID = configFromServer.identification;
        printf("My ID: %d \n", client.clientID);
    }

    /* timestamp, funete, posIni, velocidad, posFin, idCli*/
    /*inicialmente: timestamp, idCli */
    bzero(buffer,256);
    bytes = Recv(client.socketFileDescriptor,buffer,256,0);
    configFromServer = processMesage(buffer);
    while( strcmp(configFromServer.action, "start") != 0  ){
        if( strcmp(configFromServer.action, "exit") == 0){
            close(client.socketFileDescriptor);
            return 0;
        }

        bzero(buffer,256);
        bytes = Recv(client.socketFileDescriptor,buffer,256,0);
        configFromServer = processMesage(buffer);

    }

    printf("Action:%s ,StartTime:%llu,ClientPosX:%d,ClientPosY:%d,Song:%d,SongPosX:%d,SongPosY:%d\n",
           configFromServer.action,
           configFromServer.startTime,
           configFromServer.clientPosX,
           configFromServer.clientPosY,
           configFromServer.song,
           configFromServer.songPosX,
           configFromServer.songPosY);


    playerArguments.timeToStart = configFromServer.startTime;
    playerArguments.client_pos={configFromServer.clientPosX,configFromServer.clientPosY};

    playerArguments.songPos = (int **) malloc(speakersConfig.speakers_number *sizeof(int*));
    int j;
    if(configFromServer.song == -1){
        for(j=0; j<playerArguments.sound.sounds_number; j++){
            speakerPosL[j] = {configFromServer.songPosX,configFromServer.songPosY,};
        }
    }

    //double **resultWFS = WFS(x,y);

    //buffer modified to read player
    playerArguments.bufferToPlay = (void **) malloc (playerArguments.speakers.chanels_number*sizeof(void *));

    if(playerArguments.IDPlaying == client.clientID){
        playerArguments.flag = TRUE;
    }else{
        playerArguments.flag = FALSE;
    }

    // Make sure it can be shared across processes
    pthread_mutexattr_t shared;
    pthread_mutexattr_init(&shared);
    pthread_mutexattr_setpshared(&shared, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&playerArguments.lock, &shared);

    long long delay = (playerArguments.timeToStart - (playerArguments.timeToStrartSeconds * 1000) ) - current_timestamp();
    printf("Delay (10s program): %lld\n",delay);

    /*int x = 0;*/
    /* create a second thread which executes inc_x(&x) */
    if(pthread_create(&playerThread, NULL, playSuperWav, &playerArguments)) {
        fprintf(stderr, "Error creating player thread\n");
        return 1;
    }

    Player newPlayerArguments;

    getConfig(&newPlayerArguments, configFile);


    for(;;) {
        bzero(buffer, 256);
        bytes = Recv(client.socketFileDescriptor, buffer, 255, 0);

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
};
/**********************************************************************/
/* FIN CLIENT */
/**********************************************************************/