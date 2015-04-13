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
    int bytes;
    char buffer[1024];

    bzero(buffer,1024);
    bytes = Recv(sock,buffer,1024,0);

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

    bytes = Send(sock,"I got your message",18);
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

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    /* ************************************* */

    while(1){

        sd = server.newSocketFileDescriptor[cont];

        //if valid socket descriptor then add to read list
        if(sd > 0)
            FD_SET( sd , &readfds);

        //highest file descriptor number, need it for the select function
        if(sd > max_sd)
            max_sd = sd;

        timeout.tv_sec = 20;
        timeout.tv_usec = 0;

        rv = Select(server.socketFileDescriptor+cont,&readfds,NULL,NULL,&timeout);

        if(rv == -1)
        {
            perror("select error"); /// an error accured
            return 1;
        }
        else if(rv == 0)
        {
            printf("timeout occurred (20 second) \n"); // a timeout occured
            //return 1;
        }
        else {
            if (FD_ISSET(server.socketFileDescriptor, &readfds))
            {
                server.newSocketFileDescriptor[cont] = Accept(server.socketFileDescriptor,
                                                              (struct sockaddr *) &server.cli_addr,
                                                              &server.clientLenght);
                //inform user of socket number - used in send and receive commands
                printf("New connection , socket fd is %d , ip is : %s , port : %d \n" ,
                       server.newSocketFileDescriptor[cont], inet_ntoa(server.cli_addr.sin_addr) , ntohs(server.cli_addr.sin_port));

                char *message = "ECHO Daemon v1.0 \r\n";

                if( Send(server.newSocketFileDescriptor[cont], message, strlen(message), 0) != strlen(message) )
                {
                    perror("send");
                }

                printf("Welcome message sent successfully.\n");

                pid = fork();
                if (pid < 0) {
                    error("ERROR on fork");
                }
                if (pid == 0) {
                    Close(server.socketFileDescriptor);
                    dostuff(server.newSocketFileDescriptor[cont]);
                    exit(0);
                }
                else {
                    close(server.newSocketFileDescriptor[cont]);
                }

            }
            cont +=1;
        }
    }
    //close(server.newSocketFileDescriptor);
    //close(server.socketFileDescriptor);
    return 0;

}