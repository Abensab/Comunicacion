#include "./server.h"
/*  A simple server in the internet domain using TCP
    The port number is passed as an argument
    http://www.linuxhowtos.org/C_C++/socket.htm
*/


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

//    int valorBooL = 0;
//    while (valorBooL != 1){
//        if( (time +(-1*delay) - current_timestamp()) <= 0 ){ /*Experimental +(-1*delay)*/
//            playSuperWav();
    //           valorBooL = 1;
    //       }
    //   }

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

    server.socketFileDescriptor = Socket(AF_INET, SOCK_STREAM, 0);

    bzero((char *) &server.serv_addr, sizeof(server.serv_addr));

    server.serv_addr.sin_family = AF_INET;
    server.serv_addr.sin_addr.s_addr = INADDR_ANY;
    server.serv_addr.sin_port = htons(portNumber);

    Bind(server.socketFileDescriptor, (struct sockaddr *) &server.serv_addr, sizeof(server.serv_addr));

    Listen(server.socketFileDescriptor,5);
    server.clientLenght = sizeof(server.cli_addr);

    return server;
}

int startServerConnection(int portNumber){
    int pid;
    ServerConnection server = startConfigurationServer(portNumber);

    while(1){
        server.newSocketFileDescriptor = Accept(server.socketFileDescriptor, (struct sockaddr *) &server.cli_addr, &server.clientLenght);

        pid = fork();
        if (pid < 0){
            error("ERROR on fork");
        }
        if (pid == 0)  {
            Close(server.socketFileDescriptor);
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