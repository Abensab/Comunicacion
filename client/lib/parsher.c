//
// Created by mihai on 24/06/15.
//

#include "./../include/parsher.h"

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
        //printf("%s, %s\n",result[0],result[1]);

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
            resultToReturn.clientPosX = atof(result[1]);
        }

        if(strcmp(result[0],"ClientPosY") == 0) {
            resultToReturn.clientPosY = atof(result[1]);
        }

        if(strcmp(result[0],"Song") == 0) {
            resultToReturn.song = atoi(result[1]);
        }

        if(strcmp(result[0],"SongPosX") == 0) {
            resultToReturn.songPosX = atof(result[1]);
        }

        if(strcmp(result[0],"SongPosY") == 0) {
            resultToReturn.songPosY = atof(result[1]);
        }
    }

    return resultToReturn;
}