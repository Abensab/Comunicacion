#include "./client.h"
/*  A simple server in the internet domain using TCP
    The port number is passed as an argument
    http://www.linuxhowtos.org/C_C++/socket.htm
*/

ClientConnection startConfigurationClient(char *address, int portNumber){

    ClientConnection client;

    client.socketFileDescriptor = Socket(AF_INET, SOCK_STREAM, 0);
    //fcntl(client.socketFileDescriptor, F_SETFL, O_NONBLOCK);

    client.server = GetHostByName(address);

    bzero((char *) &client.server_addr, sizeof(client.server_addr));
    client.server_addr.sin_family = AF_INET;
    bcopy((char *) client.server->h_addr, (char *) &client.server_addr.sin_addr.s_addr, client.server->h_length);
    client.server_addr.sin_port = htons(portNumber);

    Connect(client.socketFileDescriptor, (struct sockaddr *) &client.server_addr, sizeof(client.server_addr));

    return client;
}

/**********************************************************************/
/* START CLIENT */
/**********************************************************************/

int startClientConnection(char *address, int portNumber){
    int                 bytes;
    char                buffer[256];
    ClientConnection    client  = startConfigurationClient(address, portNumber);

    int *pfd = startPipeFileDescriptor();

    bzero(buffer,256);
    bytes = Recv(client.socketFileDescriptor,buffer,256,0);

    printf("Milliseconds message recived: %lld\n", current_timestamp());

    long long int timeToStart = atoll(buffer);
    long long delay = (timeToStart-10000) - current_timestamp();

    printf("Moment to start: %lld\n",atoll(buffer));
    printf("Delay (10s program): %lld\n",delay);

    bzero(buffer,256);
    bytes = Recv(client.socketFileDescriptor,buffer,256,0);
    int flag = atoll(buffer);

    playSuperWav(timeToStart,flag,pfd);

    for(;;) {
        bzero(buffer, 256);
        bytes = Recv(client.socketFileDescriptor, buffer, 255, 0);

        if(bytes>0){
            int newflag = atoll(buffer);
            if(flag>=0){
                if(newflag != flag){
                    flag = newflag;
                    sendingFlag(pfd,flag);
                    //printf("Milliseconds message Recived: %lld\n", current_timestamp());
                }/*Mientras nada nuevo*/
            }
            if(newflag == -1){
                printf("\nSe acabo!!\n");
                closeWritingPipe(pfd);
                exit(0);
            }
        }
    }
    close(client.socketFileDescriptor);

    return 0;
}
/**********************************************************************/
/* FIN CLIENT */
/**********************************************************************/