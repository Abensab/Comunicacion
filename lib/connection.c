#include "./connection.h"

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

        char *args[] = {"./lib/superwav5", (char *) 0 };
        execv("./lib/superwav5", args);

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
    long long delay = (time-10000) - current_timestamp();

    printf("Moment to start: %lld\n",atoll(buffer));
    printf("Delay: %lld\n",delay);

    /*Write in a txt the delay of time*/
    //writeTimeDelay(delay);

    /*Play in that instant moment the message has been recived*/
    //playSuperWav();

    /*Play with a delay of 3 seconds*/

    int valorBooL = 0;
    while (valorBooL != 1){
        if( (time +(-1*delay) - current_timestamp()) <= 0 ){ /*Experimental +(-1*delay)*/
            playSuperWav();
            valorBooL = 1;
        }
    }

    n = write(sock,"I got your message",18);
    if (n < 0){
        error("ERROR writing to socket");
    }
}

int serverConnection(int portNumber){
    int socketFileDescriptor, newSocketFileDescriptor, pid;
    socklen_t clientLenght;
    char buffer[256];

    /*Clase de socket addres*/
    struct sockaddr_in serv_addr, cli_addr;

    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFileDescriptor < 0) {
        error("ERROR opening socket");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portNumber);

    if (bind(socketFileDescriptor, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }

    listen(socketFileDescriptor,5);
    clientLenght = sizeof(cli_addr);

    while(1){
        newSocketFileDescriptor = accept(socketFileDescriptor, (struct sockaddr *) &cli_addr, &clientLenght);
        if (newSocketFileDescriptor < 0){
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
            close(socketFileDescriptor);
            dostuff(newSocketFileDescriptor);
            exit(0);
        }
        else {
            close(newSocketFileDescriptor);
        }
    }
    close(newSocketFileDescriptor);
    close(socketFileDescriptor);
    return 0;

}

int clientConnection(char *address, int portNumber) {
    int socketFileDescriptor, n;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[256];

    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFileDescriptor < 0) {
        error("ERROR opening socket");
    }

    server = gethostbyname(address);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(portNumber);

    if (connect(socketFileDescriptor, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        error("ERROR connecting");
    }

    long long int time = current_timestamp() + 10000; //time + delay (s)

    char string_time[100];/*Descubrir tamaño aproximado*/

    sprintf( string_time, "%lld", time );

    n = write(socketFileDescriptor,&string_time,strlen(string_time));
    if (n < 0){
        error("ERROR writing to socket");
    }

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

    bzero(buffer, 256);
    n = read(socketFileDescriptor, buffer, 255);
    printf("Milliseconds message Recived: %lld\n", current_timestamp());
    if (n < 0) {
        error("ERROR reading from socket");
    }
    printf("%s\n",buffer);

    close(socketFileDescriptor);

    return 0;
}
