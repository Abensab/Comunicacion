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

/******** sendTimeToStart() *********************
 There is a separate instance of this function
 for each connection.  It handles all communication
 once a connnection has been established and sends
 the time that the superwav player has to start.
 *****************************************/
void sendTimeToStart(int sock, long long timeToStart){
    long long int   timer       =   timeToStart;
    char            string_time[64];/*Descubrir tamaño aproximado*/
    sprintf( string_time, "%lld", timer );
    //printf("Sending time %s, to socket %d \n",string_time, sock);
    Send(sock,&string_time,strlen(string_time),0);
}

void sendFlag(int sock, int flag){
    char    stringflag[64];
    sprintf( stringflag, "%d", flag );
    //printf("Flag %s,sended to socket %d \n",stringflag, sock);
    Send(sock,&stringflag,strlen(stringflag),0);

}

/* **********************************************************************
* Returns a struct with the server configuration needed,
* so in other parts of the code you can be able to send and recive messeges
* *********************************************************************** */
ServerConnection startConfigurationServer(int portNumber){
    int opt = TRUE;

    ServerConnection server;
    server.socketFileDescriptor = Socket(AF_INET, SOCK_STREAM, 0);

    //set master socket to allow multiple connections , this is just a good habit, it will work without this
    if( setsockopt( server.socketFileDescriptor, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    server.max_clients = 30;
    //initialise all client_socket[] or newFileDescriptor to 0 so not checked
    int i;
    for (i = 0; i < server.max_clients; i++)
    {
        server.newSocketFileDescriptor[i] = 0;
    }

    //fcntl(server.socketFileDescriptor, F_SETFL, O_NONBLOCK);

    bzero((char *) &server.serv_addr, sizeof(server.serv_addr));

    server.serv_addr.sin_family = AF_INET;
    server.serv_addr.sin_addr.s_addr = INADDR_ANY;
    server.serv_addr.sin_port = htons(portNumber);

    Bind(server.socketFileDescriptor, (struct sockaddr *) &server.serv_addr, sizeof(server.serv_addr));

    Listen(server.socketFileDescriptor,5);
    printf("Waiting for connections ...\n");
    server.clientLenght = sizeof(server.cli_addr);

    return server;
}