/* A simple server in the internet domain using TCP
   The port number is passed as an argument
   http://www.linuxhowtos.org/C_C++/socket.htm
   */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds
    return milliseconds;
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
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

void  writeTimeDelay(long long delayTime){
    FILE *fp;
    char dd[100];
    fp = fopen("./timeDellay.txt", "a+W" );
    if (fp==NULL)
    {
        printf("Error al abrir el archivo \n");
    }
    //printf("\t%lld\t%lld\n", current_timestamp(),delayTime);
    sprintf(dd,"\t%lld\t%lld\n", current_timestamp(),delayTime);
    fputs(dd,fp);

    fflush(fp);
    fclose(fp);
}


/******** DOSTUFF() *********************
 There is a separate instance of this function
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (int sock)
{
    int n;
    char buffer[1024];

    bzero(buffer,1024);
    n = read(sock,buffer,1024);
    if (n < 0){
        error("ERROR reading from socket");
    }
    printf("Milliseconds message recived: %lld\n", current_timestamp());

    long long time = atoll(buffer);
    long long delay = (time-3000) - current_timestamp();

    printf("Moment to start: %lld\n",atoll(buffer));
    printf("Delay: %lld\n",delay);

    /*Write in a txt the delay of time*/
    //writeTimeDelay(delay);

    /*Play in that instant moment the message has been recived*/
    //playSuperWav();

    /*Play with a delay of 3 seconds*/

    int valorBooL = 0;
    while (valorBooL != 1){
        if( (time - current_timestamp()) <= 0 ){
            playSuperWav();
            valorBooL = 1;
        }
    }

    n = write(sock,"I got your message",18);
    if (n < 0){
        error("ERROR writing to socket");
    }
}

int main(int argc, char *argv[])
{

    int sockfd, newsockfd, portno, pid;
    socklen_t clilen;
    char buffer[256];

    struct sockaddr_in serv_addr, cli_addr;
    int n;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }

    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while(1){
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0){
            error("ERROR on accept");
        }else{
            printf("Milliseconds connection made: %lld\n", current_timestamp());
            printf("Conection made from: %d.%d.%d.%d\n",
                    cli_addr.sin_addr.s_addr&0xFF,
                    (cli_addr.sin_addr.s_addr&0xFF00)>>8,
                    (cli_addr.sin_addr.s_addr&0xFF0000)>>16,
                    (cli_addr.sin_addr.s_addr&0xFF000000)>>24);
        }
        pid = fork();
        if (pid < 0){
            error("ERROR on fork");
        }
        if (pid == 0)  {
            close(sockfd);
            dostuff(newsockfd);
            exit(0);
        }
        else {
            close(newsockfd);
        }
    }
    close(newsockfd);
    close(sockfd);
    return 0;
}
