#include "./connection.h"
/*  A simple server in the internet domain using TCP
    The port number is passed as an argument
    http://www.linuxhowtos.org/C_C++/socket.htm
*/
long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds
    return milliseconds;
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int playSuperWav(){
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
void dostuff (int sock){
    int n;
    int numberCharReaded;
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
        if( (time +(-1*delay) - current_timestamp()) <= 0 ){ /*Experimental +(-1*delay)*/
            playSuperWav();
            valorBooL = 1;
        }
    }

    numberCharReaded = write(sock,"I got your message",18);
    if (numberCharReaded < 0){
        error("ERROR writing to socket");
    }
}

/* **********************************************************************
* Returns a struct with the server configuration needed,
* so in other parts of the code you can be able to send and recive messeges
* *********************************************************************** */
ServerConnection startConfigurationServer(int portNumber){
    ServerConnection server;

    server.socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (server.socketFileDescriptor < 0) {
        error("ERROR opening socket");
    }

    bzero((char *) &server.serv_addr, sizeof(server.serv_addr));

    server.serv_addr.sin_family = AF_INET;
    server.serv_addr.sin_addr.s_addr = INADDR_ANY;
    server.serv_addr.sin_port = htons(portNumber);

    if (bind(server.socketFileDescriptor, (struct sockaddr *) &server.serv_addr, sizeof(server.serv_addr)) < 0) {
        error("ERROR on binding");
    }

    listen(server.socketFileDescriptor,5);
    server.clientLenght = sizeof(server.cli_addr);

    return server;
}

int serverConnection(int portNumber){
    int pid;
    ServerConnection server = startConfigurationServer(portNumber);

    while(1){
        server.newSocketFileDescriptor = accept(server.socketFileDescriptor, (struct sockaddr *) &server.cli_addr, &server.clientLenght);
        if (server.newSocketFileDescriptor < 0){
            error("ERROR on accept");
        }else{
            printf("Milliseconds connection made: %lld\n", current_timestamp());
            printf("Conection made from: %d.%d.%d.%d\n",
                    server.cli_addr.sin_addr.s_addr&0xFF,
                    (server.cli_addr.sin_addr.s_addr&0xFF00)>>8,
                    (server.cli_addr.sin_addr.s_addr&0xFF0000)>>16,
                    (server.cli_addr.sin_addr.s_addr&0xFF000000)>>24);
        }
        pid = fork();
        if (pid < 0){
            error("ERROR on fork");
        }
        if (pid == 0)  {
            close(server.socketFileDescriptor);
            dostuff(server.newSocketFileDescriptor);
            exit(0);
        }
        else {
            close(server.newSocketFileDescriptor);
        }
    }
    close(server.newSocketFileDescriptor);
    close(server.socketFileDescriptor);
    return 0;

}
ClientConnection startConfigurationClient(char *address, int portNumber){

    ClientConnection client;

    client.socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (client.socketFileDescriptor < 0) {
        error("ERROR opening socket");
    }

    client.server = gethostbyname(address);
    if (client.server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    bzero((char *) &client.server_addr, sizeof(client.server_addr));
    client.server_addr.sin_family = AF_INET;
    bcopy((char *) client.server->h_addr, (char *) &client.server_addr.sin_addr.s_addr, client.server->h_length);
    client.server_addr.sin_port = htons(portNumber);

    if (connect(client.socketFileDescriptor, (struct sockaddr *) &client.server_addr, sizeof(client.server_addr)) < 0) {
        error("ERROR connecting");
    }
    return client;
}

int clientConnection(char *address, int portNumber){
    int numberCharReaded;

    char buffer[256];

    ClientConnection client = startConfigurationClient(address, portNumber);

    long long int time = current_timestamp() + 3000; //time + delay (s)

    char string_time[100];/*Descubrir tamaño aproximado*/

    sprintf( string_time, "%lld", time );

    numberCharReaded = write(client.socketFileDescriptor,&string_time,strlen(string_time));
    if (numberCharReaded < 0){
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
    numberCharReaded = read(client.socketFileDescriptor, buffer, 255);
    printf("Milliseconds message Recived: %lld\n", current_timestamp());
    if (numberCharReaded < 0) {
        error("ERROR reading from socket");
    }
    printf("%s\n",buffer);

    close(client.socketFileDescriptor);

    return 0;
}
