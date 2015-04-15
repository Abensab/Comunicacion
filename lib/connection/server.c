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
void sendTimeToStart(int sock, int timeToStart){
    int             bytes;
    char            buffer[256];
    long long int   timer    = current_timestamp() + (timeToStart*1000);

    char            string_time[64];/*Descubrir tamaño aproximado*/

    sprintf( string_time, "%lld", timer );

    printf("Sending time %s, to socket %d \n",string_time, sock);

    bytes = Send(sock,&string_time,strlen(string_time),0);

    /*Write in a txt the delay of time*/
    //writeTimeDelay(delay);

    /*Play in that instant moment the message has been recived*/
    //playSuperWav();

    /*Play with a delay of 3 seconds*/

    int valorBooL = 0;
    while (valorBooL != 1){
        if( (timer - current_timestamp()) <= 0 ){
            playSuperWav();
            valorBooL = 1;
        }
    }

    //bzero(buffer,256);
    //bytes = Recv(sock,buffer,256,0);

    //bytes = Send(sock,"I got your message",18);
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

int startServerConnection(int portNumber){
    int pid;
    int sd,max_sd;
    int cont = 0;
    ServerConnection server = startConfigurationServer(portNumber);

    /* For select */
    fd_set readfds;
    struct timeval timeout;
    int rv;
    //clear the socket set
    FD_ZERO(&readfds);
    //add our file descriptor to the set
    FD_SET(server.socketFileDescriptor, &readfds);
    max_sd = server.socketFileDescriptor;
    cont = cont+1;

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    /* ************************************* */

    while(1) {
        int search = TRUE;

        if(search){
            sd = server.newSocketFileDescriptor[cont];

            //if valid socket descriptor then add to read list
            if (sd > 0)
                FD_SET(sd, &readfds);

            //highest file descriptor number, need it for the select function
            if (sd > max_sd)
                max_sd = sd;

            rv = Select(server.socketFileDescriptor + cont, &readfds, NULL, NULL, &timeout);

            if (rv == -1) {
                perror("select error"); /// an error accured
                return 1;
            }
            else if (rv == 0) {
                //printf(" ************************** timeout occurred (3 second) **************************\n"); // a timeout occured
                //return 1;
                timeout.tv_sec = 1;
                timeout.tv_usec = 0;

                /*Hay que inicializar estos datos cada vez que se utiliza resetea la configuración*/
                //add our file descriptor to the set
                FD_SET(server.socketFileDescriptor, &readfds);
                /**/
            }
            else {
                if (FD_ISSET(server.socketFileDescriptor, &readfds)) {
                    server.newSocketFileDescriptor[cont] = Accept(server.socketFileDescriptor,
                                                                  (struct sockaddr *) &server.cli_addr,
                                                                  &server.clientLenght);
                    //inform user of socket number - used in send and receive commands
                    printf("New connection , socket fd is %d , ip is : %s , port : %d \n",
                           server.newSocketFileDescriptor[cont], inet_ntoa(server.cli_addr.sin_addr),
                           ntohs(server.cli_addr.sin_port));

                    /*char *message = "ECHO Daemon v1.0 \r\n";

                    Send(server.newSocketFileDescriptor[cont], message, strlen(message), 0);

                    printf("Welcome message sent successfully.\n");*/

                }
                cont += 1;
            }
        }

        if (kbhit()) {
            char c = getchar();
            printf("has presionado %c\n", c);

            if (c == 's') {
                search = FALSE;
                printf("START music in 10s!\n");
                int i;
                for (i = 0; i < server.max_clients; i++)
                {
                    if (server.newSocketFileDescriptor[i]!= 0){
                        printf("Connection, cont = %d, %d \n",i, server.newSocketFileDescriptor[i]);
                        pid = fork();
                        if (pid < 0) {
                            error("ERROR on fork");
                        }
                        if (pid == 0) {
                            Close(server.socketFileDescriptor);
                            sendTimeToStart(server.newSocketFileDescriptor[i],10);
                            exit(0);
                        }
                        else {
                            close(server.newSocketFileDescriptor[i]);
                            server.newSocketFileDescriptor[i] = 0;
                        }
                    }
                    else
                        printf("No connection, cont = %d \n",i);
                }
            }
        }
    }
    //close(server.newSocketFileDescriptor);
    //close(server.socketFileDescriptor);
    return 0;

}