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

    printf("Action:%s,StartTime:%llu,ClientPosX:%d,ClientPosY:%d,Song:%d,SongPosX:%d,SongPosY:%d\n",
           configFromServer.action,
           configFromServer.startTime,
           configFromServer.clientPosX,
           configFromServer.clientPosY,
           configFromServer.song,
           configFromServer.songPosX,
           configFromServer.songPosY);

    playerArguments.timeToStart = configFromServer.startTime;
    playerArguments.client_pos[0] = configFromServer.clientPosX;
    playerArguments.client_pos[0] = configFromServer.clientPosY;

    int j;
    playerArguments.songPos = (int **) malloc(playerArguments.speakers.speakers_number *sizeof(int*));
    if(configFromServer.song == -1){
       for(j=0; j<playerArguments.sound.sounds_number; j++){
            playerArguments.songPos[j] = (int *) malloc(2 *sizeof(int));

            playerArguments.songPos[j][0] = configFromServer.songPosX;
            playerArguments.songPos[j][1] = configFromServer.songPosY;
        }
    }

    //double **resultWFS = WFS(x,y);

    //buffer modified to read player
    playerArguments.bufferToPlay = (void **) malloc (playerArguments.speakers.chanels_number*sizeof(void *));

    //desaparece y se cambia por el WFS
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
    printf("Delay (%d seconds program): %lld\n",playerArguments.timeToStrartSeconds,delay);

    /*int x = 0;*/
    /* create a second thread which executes inc_x(&x) */
/*    if(pthread_create(&playerThread, NULL, playSuperWav, &playerArguments)) {
        fprintf(stderr, "Error creating player thread\n");
        return 1;
    }
*/
    //New playaer that will listen and save all the new configurations that the server ahs sended.
    Player newPlayerArguments;
    newPlayerArguments.timeToStart = playerArguments.timeToStart;
    getConfig(&newPlayerArguments, configFile);
    newPlayerArguments.client_pos = playerArguments.client_pos;
    newPlayerArguments.finishPlaying = playerArguments.finishPlaying;

    //Copy of the player arguments for the songs pozition.
    newPlayerArguments.songPos = (int **) malloc(newPlayerArguments.speakers.speakers_number *sizeof(int*));
    for(j=0; j<newPlayerArguments.sound.sounds_number; j++){
        newPlayerArguments.songPos[j] = (int *) malloc(2 *sizeof(int));
        newPlayerArguments.songPos[j][0] = playerArguments.songPos[j][0];
        newPlayerArguments.songPos[j][1] = playerArguments.songPos[j][1];
    }

    MesageVariables newConfigFromServer;

    for(;;) {
        bzero(buffer, 256);
        bytes = Recv(client.socketFileDescriptor, buffer, 255, 0);
        fflush(stdout);

        if(bytes>0){
            fflush(stdout);
            newConfigFromServer = processMesage(buffer);

            if( strcmp(newConfigFromServer.action, "exit") == 0){

                printf("See youn soon!\n");
                break;
            }

            if (strcmp(newConfigFromServer.action, "song") == 0){

                if(newConfigFromServer.song == -1){

                    for(j=0; j<newPlayerArguments.sound.sounds_number; j++){

                        newPlayerArguments.songPos[j][0] = newConfigFromServer.songPosX;
                        newPlayerArguments.songPos[j][1] = newConfigFromServer.songPosY;
                    }
                }else{

                    if(newConfigFromServer.song < newPlayerArguments.sound.sounds_number){

                        newPlayerArguments.songPos[newConfigFromServer.song][0] = newConfigFromServer.songPosX;
                        newPlayerArguments.songPos[newConfigFromServer.song][1] = newConfigFromServer.songPosY;

                    }else{

                        printf("Error, Sound %d dose not exist. Maxim number of sounds: %d", newConfigFromServer.song,
                               newPlayerArguments.sound.sounds_number);
                    }
                }
                printf("Sound: %d, PosX: %d, PosY: %d \n", newConfigFromServer.song, newConfigFromServer.songPosX,
                       newConfigFromServer.songPosY);

            }

            if (strcmp(newConfigFromServer.action, "client") == 0){
                printf("Client => PosX: %d, PosY: %d \n", newConfigFromServer.clientPosX,newConfigFromServer.clientPosY);
                printf("No disponible actualmente\n");
            }

/*
            printf("Aplicando nuevos cambios al buffer");

            if(newPlayerArguments.IDPlaying == client.clientID){
                pthread_mutex_lock(&playerArguments.lock);
                playerArguments.flag = TRUE;
                printf("Son iguales\n");
                pthread_mutex_unlock(&playerArguments.lock);
            }else{
                pthread_mutex_lock(&playerArguments.lock);
                playerArguments.flag = FALSE;
                printf("NO son iguales\n");
                pthread_mutex_unlock(&playerArguments.lock);
            }
            if(newPlayerArguments.IDPlaying == -1){
                printf("\n¡¡Se acabo!!\n");
                playerArguments.flag = -1;
                break;
            }
*/
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