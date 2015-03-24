#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h> /* for wait */

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds
    //printf("%lld \n",milliseconds);
    return milliseconds;
}

int playSuperWav() {
    fprintf (stdout, "**** File Started at: %lld ****\n",current_timestamp());

    pid_t pid=fork();
    if (pid==0) {

        char *args[] = {"./bin/superwav5", (char *) 0 };
        execv("./bin/superwav5", args);

        exit(-1);
    }
    else {
        waitpid(pid,0,0);
    }
    return 0;
}

int main(int argc, char *argv[]) {

    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int n;

    char buffer[256];
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    long long int time = current_timestamp() + 3000; //time + delay (s)
    char string_time[1000];
    sprintf( string_time, "%lld", time );

    n = write(sockfd,&string_time,strlen(string_time));
    if (n < 0){
        error("ERROR writing to socket");
    }

    /*printf("Please enter the message: ");
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);

    //current time stamp
    printf("Milliseconds message sent: %lld\n", current_timestamp());

    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        error("ERROR writing to socket");
    }
*/
    int valorBooL = 0;
    while (valorBooL != 1){
        //printf("Time to start: %lld \n",time - current_timestamp());
        //printf("%d \n",valorBooL);
        if( (time - current_timestamp()) <= 0 ){
            playSuperWav();
            valorBooL = 1;
        }
        //sleep(1);
    }

    bzero(buffer, 256);
    n = read(sockfd, buffer, 255);
    printf("Milliseconds message Recived: %lld\n", current_timestamp());
    if (n < 0) {
        error("ERROR reading from socket");
    }
    printf("%s\n",buffer);

    close(sockfd);

    return 0;
}