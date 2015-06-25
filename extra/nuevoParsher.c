//
// Created by mihai on 24/06/15.
//

#include "nuevoParsher.h"

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

/*int setArguments(char *str, Player* playerArguments, char* configFile){

    //Config reader
    config_t            cfg;
    checkConfig(&cfg,configFile);

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
            playerArguments->timeToStart= atoll(variableORValue[1]);
        }

        if(i == 1){
            playerArguments->IDPlaying= atoi(variableORValue[1]);
        }

        free(variableORValue[0]);
        free(variableORValue[1]);
        free(variableAndValue[i]);
    }
    playerArguments->card = clientCardGenerator(&cfg);
    playerArguments->sound = clientSoundGenerator(&cfg);
    playerArguments->speekers = clientSpeekersGenerator(&cfg);
    playerArguments->timeToStrartSeconds = timeToStartConfig(&cfg);

    return 0;
}
*/

char **getVariableAndValue(char** str, int index) {
    // Always returns variable:value
    //[[variable][value]]
    char **variableORValue;
    size_t twoValues;
    return variableORValue = strsplit(str[index], ":", &twoValues);
}

MesageVariables processMesage(char* str){

    MesageVariables resultToReturn;
    resultToReturn.action = "";

    // For sliting
    char**  resultadoSplit;
    size_t  numeroDeValoresEnCadena;
    int i;

    resultadoSplit = strsplit(str, ",", &numeroDeValoresEnCadena);

    for(i = 0; i < (int)numeroDeValoresEnCadena ;i++){
        char** result = getVariableAndValue(resultadoSplit, i);
        printf("%s, %s\n",result[0],result[1]);

        if(strcmp(result[0],"Action") == 0) {
            resultToReturn.action = result[1];
        }

        if(strcmp(result[0],"ID") == 0) {
            resultToReturn.identification = atoi(result[1]);
        }

        if(strcmp(result[0],"StartTime") == 0) {
            resultToReturn.startTime = atoll(result[1]);
        }

        if(strcmp(result[0],"ClientPosX") == 0) {
            resultToReturn.clientPosX = atoi(result[1]);
        }

        if(strcmp(result[0],"ClientPosY") == 0) {
            resultToReturn.clientPosY = atoi(result[1]);
        }

        if(strcmp(result[0],"Song") == 0) {
            resultToReturn.song = atoi(result[1]);
        }

        if(strcmp(result[0],"SongPosX") == 0) {
            resultToReturn.songPosX = atoi(result[1]);
        }

        if(strcmp(result[0],"SongPosY") == 0) {
            resultToReturn.songPosY = atoi(result[1]);
        }
    }
/*
    if(strcmp(resultToReturn.action,"identification") == 0){
        printf("Action:%s ,ID:%d\n",resultToReturn.action,resultToReturn.identification);
    }

    if(strcmp(resultToReturn.action,"start") == 0){
        printf("Action:%s ,StartTime:%llu,ClientPosX:%d,ClientPosY:%d,Song:%d,SongPosX:%d,SongPosY:%d\n",
               resultToReturn.action,resultToReturn.startTime,resultToReturn.clientPosX,resultToReturn.clientPosY,
               resultToReturn.song,resultToReturn.songPosX,resultToReturn.songPosY);
    }

    if(strcmp(resultToReturn.action,"client") == 0){
        printf("Action:%s ,StartTime:%llu,ClientPosX:%d,ClientPosY:%d\n",resultToReturn.action,resultToReturn.startTime,
               resultToReturn.clientPosX,resultToReturn.clientPosY);
    }

    if(strcmp(resultToReturn.action,"song") == 0){
        printf("Action:%s ,StartTime:%llu,Song:%d,SongPosX:%d,SongPosY:%d\n",
               resultToReturn.action,resultToReturn.startTime,resultToReturn.song,resultToReturn.songPosX,
               resultToReturn.songPosY);
    }

    if(strcmp(resultToReturn.action,"exit") == 0){
        printf("Action:%s",resultToReturn.action);
    }
*/
    return resultToReturn;
}

int main()
{
    char* msg[5]={NULL,NULL,NULL,NULL,NULL};

    msg[0] = "Action:identification,ID:1";
    msg[1] = "Action:start,StartTime:1435142177654,ClientPosX:6,ClientPosY:5,Song:-1,SongPosX:11,SongPosY: 0";
    msg[2] = "Action:client,StartTime:0,ClientPosX:7,ClientPosY:5";
    msg[3] = "Action:song,StartTime:0,Song:1,SongPosX:5,SongPosY:0";
    msg[4] = "Action:exit";


    printf("== %s ==\n",msg[0]);
    printf("\n");
    processMesage(msg[0]);
    printf("\n");
    printf("== %s ==\n",msg[1]);
    printf("\n");
    processMesage(msg[1]);
    printf("\n");
    printf("== %s ==\n",msg[2]);
    printf("\n");
    processMesage(msg[2]);
    printf("\n");
    printf("== %s ==\n",msg[3]);
    printf("\n");
    processMesage(msg[3]);
    printf("\n");
    printf("== %s ==\n",msg[4]);
    printf("\n");
    processMesage(msg[4]);


    return 0;
}