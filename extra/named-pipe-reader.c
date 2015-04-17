//http://staff.neu.edu.tr/~hsevay/fall-2010-2011/courses/com324/labs/09/com324-spring-2009-2010-lab09.pdf

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
int thereIsDataToRead( int fd ) {
    struct timeval timeout;
    fd_set readfds;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    FD_ZERO( &readfds );
    FD_SET( fd, &readfds );
    select( fd+1, &readfds, NULL, NULL, &timeout );
    return (FD_ISSET( fd, &readfds ));
} /* end thereIsDataToRead */
int main( void ) {
    int npfd=open( "/tmp/my_named_pipe", O_RDONLY | O_NONBLOCK );
    char buffer[100];
    if (npfd < 0) {
        perror( "open" );
        exit( 1 );
    }
    while (!thereIsDataToRead( npfd )) {
        printf( "--- Currently there is no data to read ...\n" );
        usleep( 500000 );
    } // end while
    read( npfd, buffer, 100 );
    printf( "%s\n", buffer );
    close( npfd );
    return( 0 );
} /* end main */