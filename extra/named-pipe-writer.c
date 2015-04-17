#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
char *phrase="This is a test for using a named pipe ...";
int main( void ) {
    int npfd=open( "/tmp/my_named_pipe", O_WRONLY );
    if (npfd < 0) {
        perror( "open" );
        exit( 1 );
    }
    write( npfd, phrase, strlen( phrase ) + 1 );
    close( npfd );
    return( 0 );
} /* end main */