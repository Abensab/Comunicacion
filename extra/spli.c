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

/*
int main(void) {
    char *line = NULL;
    size_t linelen;

    char **tokens;
    size_t numtokens;

    line ="1,patata,616981681";
    linelen= strlen(line);

    tokens = strsplit(line, ", \t\n", &numtokens);
    size_t i;
    for (i = 0; i < numtokens; i++) {
        printf("    token: \"%s\"\n", tokens[i]);
        free(tokens[i]);
    }
    if (tokens != NULL)
        free(tokens);

//    if (line != NULL) free(line);

    return EXIT_SUCCESS;

}*/

int main(void) {

    long long time = current_timestamp();
    char str[100];
    sprintf(str, "%lld", time);
    printf("%s\n",str);

    char *ptr = strcat(str, ",id 100");
    printf("%s\n",ptr);

    return 0;

}