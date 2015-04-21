#include "./pipe.h"

int* startPipeFileDescriptor(){
    /* Pipe’s file descriptors. */
    /* Set up pipe. */
    static int pfd[2];

    if (pipe(pfd) == -1){
        fprintf(stderr, "Call to pipe failed.\n");
        exit(1);
    }

    /* Set non-blocking on the readable end. */
    if ( fcntl(pfd[P1_READ], F_SETFL, O_NONBLOCK) == -1)
    {
        close(pfd[P1_READ]);
        close(pfd[P1_WRITE]);
        fprintf(stderr, "Call to fcntl failed.\n");
        exit(1);
    }

    return pfd;
}

void closePipeFileDescriptor(int p[]){
    close(p[P1_READ]);
    close(p[P1_WRITE]);
}

void sendingFlag (int p[], int val) {
    /* Code for parent process. */
    /* Send messages through the pipe. */
    //printf("\nParrent(%d) send value: %d\n", getpid(), val);
    write(p[P1_WRITE], &val, sizeof(val));

}

void closeReadingPipe(int p[]){
    // parent: writing only, so close read-descriptor.
    if (close(p[P1_READ]) == -1) { /* Failed to close read end of pipe. */
        fprintf(stderr, "Parrent: Couldn’t close read end of pipe.\n");
        exit(1);
    }
}

void closeWritingPipe(int p[]){
    // child: reading only, so close the write-descriptor
    if (close(p[P1_WRITE]) == -1) { /* Failed to close write end of pipe. */
        fprintf(stderr, "Child: Couldn’t close write end of pipe.\n");
        exit(1);
    }
}

int readFlag (int p[]) {
    /* Code for the child process. */

    int val = -1;
    int nread = read(p[P1_READ], &val, sizeof(val));
    /* Repeatedly monitor the pipe for messages. Stop when the pipe is closed. */

    if(nread == -1){
        if (errno == EAGAIN) {
            //printf("Child: Pipe is empty\n");
            fflush(stdout);
        }else { /* Reading from pipe failed. */
            fprintf(stderr, "Child: Couldn’t read from pipe.\n");
            exit(1);
        }
    }else{
        if (nread == 0){/* Pipe has been closed. */
            printf("Child: End of conversation.\n");
            fflush(stdout);
            exit(0);
        }else{/* Received a message from the pipe. */
            printf("\nChild(%d) received value: %d\n", getpid(), val);
            fflush(stdout);
        }
    }
    return val;
}