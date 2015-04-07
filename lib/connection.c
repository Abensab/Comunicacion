#include "./connection.h"
/*  A simple server in the internet domain using TCP
    The port number is passed as an argument
    http://www.linuxhowtos.org/C_C++/socket.htm
*/


/* generalizar */
int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
/* generalizar */

long long current_timestamp() {
    struct      timeval te;
    long long   milliseconds;

    gettimeofday(&te, NULL); // get current time
    milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds

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
    FILE    *fp;
    char    dd[100];

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
void dostuff (ServerConnection server){
    int     numberCharReaded;
    char    buffer[1024];

    numberCharReaded = recvfrom(server.socketFileDescriptor, buffer, 1024, 0, (struct sockaddr *)&server.cli_addr, &server.clientLenght);
    if (numberCharReaded < 0){
        error("ERROR reading from socket");
    }else{
        /*printf("Milliseconds connection made: %lld\n", current_timestamp());
        printf("Conection made from: %d.%d.%d.%d\n",
                server.cli_addr.sin_addr.s_addr&0xFF,
                (server.cli_addr.sin_addr.s_addr&0xFF00)>>8,
                (server.cli_addr.sin_addr.s_addr&0xFF0000)>>16,
                (server.cli_addr.sin_addr.s_addr&0xFF000000)>>24);
                */

        printf("Server-Waiting and listening...\n");

    }
    buffer[numberCharReaded] = '\0'; /*End of file.*/

    printf("Server-Got packet from %s\n", inet_ntoa(server.cli_addr.sin_addr));
    printf("Server-Packet is %d bytes long\n", numberCharReaded);

    printf("Server-Packet contains \"%s\"\n", buffer);

//    printf("Milliseconds message recived: %lld\n", current_timestamp());

    long long time = atoll(buffer);
    long long delay = (time-3000) - current_timestamp();

//    printf("Moment to start: %lld\n",atoll(buffer));
//    printf("Delay: %lld\n",delay);

    /*Write in a txt the delay of time*/
    //writeTimeDelay(delay);

    /*Play in that instant moment the message has been recived*/
    //playSuperWav();

    /*Play with a delay of 3 seconds*/

//    int valorBooL = 0;
//    while (valorBooL != 1){
//        if( (time +(-1*delay) - current_timestamp()) <= 0 ){ /*Experimental +(-1*delay)*/
//            playSuperWav();
 //           valorBooL = 1;
 //       }
 //   }

    numberCharReaded = sendto(server.socketFileDescriptor,"Got your message\n", 17, 0, (struct sockaddr *)&server.cli_addr, server.clientLenght);
    if (numberCharReaded < 0){
        error("ERROR writing to socket");
    }

}

int waitingClients (ServerConnection server){
    int     bytes;
    char    buffer[1024];

    bytes = recvfrom(server.socketFileDescriptor, buffer, 1024, 0, (struct sockaddr *)&server.cli_addr, &server.clientLenght);
    if (bytes < 0){
        error("ERROR reading from socket");
    }else{
        printf("Server-Waiting and listening...\n");
    }
    buffer[bytes] = '\0'; /*End of file.*/

    printf("Server-Got packet from %s\n", inet_ntoa(server.cli_addr.sin_addr));
    printf("Server-Packet is %d bytes long\n", bytes);

    printf("Server-Packet contains \"%s\"\n", buffer);

    bytes = sendto(server.socketFileDescriptor,"Ok! Got your message.\n", 17, 0, (struct sockaddr *)&server.cli_addr, server.clientLenght);
    if (bytes < 0){
        error("ERROR writing to socket");
    }
    return 0;
}


/* **********************************************************************
* Returns a struct with the server configuration needed,
* so in other parts of the code you can be able to send and recive messeges
* *********************************************************************** */
ServerConnection startConfigurationServer(int portNumber){
    ServerConnection server;

    server.socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);;
    server.portNumber = portNumber;

    if (server.socketFileDescriptor < 0) {
        error("ERROR opening socket");
    }

    int length = sizeof(server.serv_addr);
    bzero(&server.serv_addr, length);

    server.serv_addr.sin_family = AF_INET;
    server.serv_addr.sin_addr.s_addr = INADDR_ANY;
    server.serv_addr.sin_port = htons(server.portNumber);

    if (bind(server.socketFileDescriptor, (struct sockaddr *) &server.serv_addr, length) < 0) {
        error("ERROR on binding");
    }
    server.clientLenght = sizeof(struct sockaddr_in);
    return server;
}

int startServerConnection(int portNumber){
    int pid, numClient;
    ServerConnection server = startConfigurationServer(portNumber);

    for ( numClient = 0; numClient < 1; numClient++) {

        if ( (waitingClients(server)) != 0 ){
            error("Error en conectar con liente");
        }
    }

    while(1) {

        if (kbhit()) {
            char c = getchar();
            printf("has presionado %c\n", c);
            if (c == 's') {
                printf("START music!");

            }
        }
    }

/*
    while(1){
        pid = fork();
        if (pid < 0){
            error("ERROR on fork");
        }
        if (pid == 0)  {
            printf("Soy hijo tonto...\n");
            close(server.socketFileDescriptor);
            exit(0);
        }
        else {
            dostuff(server);
        }


    }
    return 0;
*/
}
ClientConnection startConfigurationClient(char *address, int portNumber){
    ClientConnection client;
    client.socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);

    if (client.socketFileDescriptor < 0) {
        error("ERROR opening socket");
    }
    client.serv_addr.sin_family = AF_INET;
    client.server = gethostbyname(address);

    if (client.server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }
    bcopy((char *) client.server->h_addr, (char *) &client.serv_addr.sin_addr, client.server->h_length);

    client.portNumber = portNumber;
    client.serv_addr.sin_port = htons(client.portNumber);

    return client;
}

int startClientConnection(char *address, int portNumber){
    int     numberCharReaded;
    char    buffer[256];

    ClientConnection client = startConfigurationClient(address, portNumber);
    client.length = sizeof(struct sockaddr_in);

    long long int time = current_timestamp() + 3000; //time + delay (s)

    char string_time[100];/*Descubrir tamaño aproximado*/

    sprintf( string_time, "%lld", time );

    numberCharReaded = sendto(client.socketFileDescriptor,&string_time, strlen(string_time),0,(const struct sockaddr *)&client.serv_addr,client.length);
    if (numberCharReaded < 0){
        error("ERROR writing to socket");
    }

    printf("sent %d bytes to %s\n", numberCharReaded, inet_ntoa(client.serv_addr.sin_addr));

    /*Play in that instant moment the message has been recived*/
    //playSuperWav();

    /*Play with a delay of 3 seconds*/

    /*
    int valorBooL = 0;
    while (valorBooL != 1){
        if( (time - current_timestamp()) <= 0 ){
            playSuperWav();
            valorBooL = 1;
        }
    }
    */

    bzero(buffer, 256);
    numberCharReaded = recvfrom(client.socketFileDescriptor,buffer,255,0,(struct sockaddr *)&client.cli_addr, &client.length);

    printf("Milliseconds message Recived: %lld\n", current_timestamp());

    if (numberCharReaded < 0) {
        error("ERROR reading from socket");
    }

    write(1,"Got an ack: ",12);
    write(1,buffer,numberCharReaded);

    close(client.socketFileDescriptor);

    return 0;
}
