#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*assignar no bloqueante a la tuberia*/
#include <fcntl.h>
/*recoger error*/
#include <errno.h>


// some macros to make the code more understandable
//  regarding which pipe to use to a read/write operation
//
//  Parent: writes on P1_WRITE
//  Child:  reads from P1_READ

#define P1_READ     0
#define P1_WRITE    1

int parent_connection (int []);
int child_superwav (int []);
/* Prototypes. */

int main(int argc, char *argv[])
{
    /* Pipe’s file descriptors. */
    /* Set up pipe. */
    int pfd[2];
    int val = 0;


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

    /* Fork a child process. */
    switch (fork()) {

        case (pid_t) -1: /* Fork failed. */
            fprintf(stderr, "Call to fork failed.\n"); exit(1);

        case 0: /* Child process. */
            child_superwav(pfd);

        default: /* Parent process. */
            parent_connection(pfd);
    } /* End of switch. */

    return 0;
}/* End of main. */

int parent_connection (int p[]) {
    /* Code for parent process. */

    int val;

    // parent: writing only, so close read-descriptor.
    if (close(p[P1_READ]) == -1) { /* Failed to close read end of pipe. */
        fprintf(stderr, "Parrent: Couldn’t close read end of pipe.\n");
        exit(1);
    }

    // send the value on the write-descriptor.
    val = 1;

    int count;
    /* Send messages through the pipe. */
    for (count = 0; count < 3; count++) {
        printf("Parrent(%d) send value: %d\n", getpid(), val);
        write(p[P1_WRITE], &val, sizeof(val));
        sleep(2);
    }

    // close the write descriptor
    close(p[P1_WRITE]);
}

int child_superwav (int p[]) {
/* Code for the child process. */

    int val;
    int nread;

    // child: reading only, so close the write-descriptor
    if (close(p[P1_WRITE]) == -1) { /* Failed to close write end of pipe. */
        fprintf(stderr, "Child: Couldn’t close write end of pipe.\n");
        exit(1);
    }

    /* Repeatedly monitor the pipe for messages. Stop when the pipe is closed. */
    for (;;) {
        // now read the data (will block)
        switch (nread = read(p[P1_READ], &val, sizeof(val))) {
            case -1: /* Make sure that pipe is empty. */
                if (errno == EAGAIN) {
                    //printf("Child: Pipe is empty\n");
                    fflush(stdout);
                    break;
                }
                else { /* Reading from pipe failed. */
                    fprintf(stderr, "Child: Couldn’t read from pipe.\n");
                    exit(1);
                }
            case 0: /* Pipe has been closed. */
                printf("Child: End of conversation.\n");
                fflush(stdout);
                exit(0);
            default: /* Received a message from the pipe. */
                printf("Child(%d) received value: %d\n", getpid(), val);
                fflush(stdout);
                break;
        } /* End of switch. */
    } /* End of for loop. */

    // close the read-descriptor
    close(p[P1_READ]);
}
