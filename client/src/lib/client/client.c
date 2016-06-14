#include "client.h"
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
    playerArguments->client_pos = (float *) malloc(2 *sizeof(float));

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
        configFile    = "../config/default.cfg";
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

    printf("Action:%s,StartTime:%llu,ClientPosX:%f,ClientPosY:%f,Song:%d,SongPosX:%f,SongPosY:%f\n",
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

    printf("Time offset: %lld\n", playerArguments.timeToStart - current_timestamp());

    long long delay = (playerArguments.timeToStart - (playerArguments.timeToStrartSeconds * 1000) ) - current_timestamp();
    printf("Delay (%d seconds program): %lld\n",playerArguments.timeToStrartSeconds,delay);

    int j;
    playerArguments.songPos = (float **) malloc( playerArguments.speakers.speakers_number * sizeof(float*));
    playerArguments.wfsVector = (WFS *) malloc( playerArguments.sound.sounds_number * sizeof(WFS));


    if(configFromServer.song == -1){
       for(j=0; j<playerArguments.sound.sounds_number; j++){
            playerArguments.songPos[j] = (float *) malloc(2 *sizeof(float));

            playerArguments.songPos[j][0] = configFromServer.songPosX;
            playerArguments.songPos[j][1] = configFromServer.songPosY;
            playerArguments.wfsVector[j] = waveFieldSynthesis(playerArguments.speakers,configFromServer.songPosX,configFromServer.songPosY);

        }
    }



    //desaparece y se cambia por el WFS
    //inicializo contador para el numero de frames.
    playerArguments.l1 = 0;

    // load songs
    playerArguments.fileWAV = loadFile(playerArguments.sound);

    // Make sure it can be shared across processes
    pthread_mutexattr_t shared;
    pthread_mutexattr_init(&shared);
    pthread_mutexattr_setpshared(&shared, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&playerArguments.lock, &shared);

    long long delay2 = (playerArguments.timeToStart - (playerArguments.timeToStrartSeconds * 1000) ) - current_timestamp();
    printf("Delay2 to start song (%d seconds program): %lld\n",playerArguments.timeToStrartSeconds,delay2);

    /*int x = 0;*/
    /* create a second thread which executes inc_x(&x) */
    if(pthread_create(&playerThread, NULL, playSuperWav, &playerArguments)) {
        fprintf(stderr, "Error creating player thread\n");
        return 1;
    }

    //New playaer that will listen and save all the new configurations that the server has sended.
    Player newPlayerArguments;
    newPlayerArguments.timeToStart = playerArguments.timeToStart;
    getConfig(&newPlayerArguments, configFile);
    newPlayerArguments.client_pos = playerArguments.client_pos;
    newPlayerArguments.finishPlaying = playerArguments.finishPlaying;


    //Copy of the player arguments for the songs pozition.
    newPlayerArguments.songPos = (float **) malloc(newPlayerArguments.speakers.speakers_number *sizeof(float*));
    newPlayerArguments.wfsVector = (WFS *) malloc( newPlayerArguments.sound.sounds_number * sizeof(WFS));

    for(j=0; j<newPlayerArguments.sound.sounds_number; j++){
        newPlayerArguments.songPos[j] = (float *) malloc(2 *sizeof(float));
        newPlayerArguments.songPos[j][0] = playerArguments.songPos[j][0];
        newPlayerArguments.songPos[j][1] = playerArguments.songPos[j][1];
        newPlayerArguments.wfsVector[j] = playerArguments.wfsVector[j];
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
                playerArguments.finishPlaying = TRUE;
                break;
            }

            if (strcmp(newConfigFromServer.action, "client") == 0){
                printf("Client => PosX: %f, PosY: %f \n", newConfigFromServer.clientPosX,newConfigFromServer.clientPosY);
                printf("No disponible actualmente\n");
            }

            if (strcmp(newConfigFromServer.action, "song") == 0){

                if(newConfigFromServer.song == -1){

                    for(j=0; j<newPlayerArguments.sound.sounds_number; j++){


                        pthread_mutex_lock(&playerArguments.lock);

                        playerArguments.songPos[j][0] = newConfigFromServer.songPosX;
                        playerArguments.songPos[j][1] = newConfigFromServer.songPosY;
                        playerArguments.wfsVector[j] = waveFieldSynthesis(playerArguments.speakers,newConfigFromServer.songPosX,newConfigFromServer.songPosY);
                        /*
                        newPlayerArguments.songPos[j][0] = newConfigFromServer.songPosX;
                        newPlayerArguments.songPos[j][1] = newConfigFromServer.songPosY;
                        newPlayerArguments.wfsVector[j] = waveFieldSynthesis(playerArguments.speakers,newConfigFromServer.songPosX,newConfigFromServer.songPosY);
                        */

                        pthread_mutex_unlock(&playerArguments.lock);

                    }
                }else{

                    if(newConfigFromServer.song < newPlayerArguments.sound.sounds_number){

                        pthread_mutex_lock(&playerArguments.lock);

                        playerArguments.songPos[newConfigFromServer.song][0] = newConfigFromServer.songPosX;
                        playerArguments.songPos[newConfigFromServer.song][1] = newConfigFromServer.songPosY;
                        playerArguments.wfsVector[newConfigFromServer.song] = waveFieldSynthesis(playerArguments.speakers,newConfigFromServer.songPosX,newConfigFromServer.songPosY);

                        pthread_mutex_unlock(&playerArguments.lock);


                        /*newPlayerArguments.songPos[newConfigFromServer.song][0] = newConfigFromServer.songPosX;
                        newPlayerArguments.songPos[newConfigFromServer.song][1] = newConfigFromServer.songPosY;
                        newPlayerArguments.wfsVector[newConfigFromServer.song] = waveFieldSynthesis(playerArguments.speakers,newConfigFromServer.songPosX,newConfigFromServer.songPosY);
                        */


                    }else{

                        printf("Error, Sound %d dose not exist. Maxim number of sounds: %d", newConfigFromServer.song,
                               newPlayerArguments.sound.sounds_number);
                    }
                }
                printf("Sound: %d, PosX: %f, PosY: %f \n", newConfigFromServer.song, newConfigFromServer.songPosX,
                       newConfigFromServer.songPosY);

                /*pthread_mutex_lock(&playerArguments.lock);
                playerArguments.songPos = newPlayerArguments.songPos;
                playerArguments.wfsVector = newPlayerArguments.wfsVector;
                pthread_mutex_unlock(&playerArguments.lock);
                */
            }

        }
        if(playerArguments.finishPlaying == TRUE){
            break;
        }
    }

    close(client.socketFileDescriptor);
    pthread_mutex_destroy(&playerArguments.lock);
    pthread_exit(NULL);

    exit(0);
};
/**********************************************************************/
/* FIN CLIENT */
/**********************************************************************/