#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

long long current_timestamp() {
    struct      timeval te;
    long long   milliseconds;

    gettimeofday(&te, NULL); // get current time
    milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds

    return milliseconds;
}

typedef struct Arg_thread_TAG{

    long long timeToStart;
    int flag;
    int finishPlaying;
    int IDPlaying;

} Arg_thread;

Arg_thread setArguments(char *str){
    Arg_thread argumets;

    size_t linelen= strlen(str);
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
        size_t j;

        if(i == 0){
            argumets.timeToStart = atoll(variableORValue[1]);
        }

        if(i == 1){
            argumets.flag= atoi(variableORValue[1]);
        }

        if(i == 2){
            argumets.IDPlaying= atoi(variableORValue[1]);
        }

        free(variableORValue[0]);
        free(variableORValue[1]);
        free(variableAndValue[i]);
    }

    return argumets;
}

int main(void) {

    char str[250] = "StartTime: 1430298639579,Flag: 0,IDClient: 1";

    Arg_thread argumets = setArguments(str);

    printf("TimeToStart %lld, Flag: %d, IDClient: %d \n",argumets.timeToStart,argumets.flag,argumets.IDPlaying);

    return 0;

}