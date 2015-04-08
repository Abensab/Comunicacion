#include "./client.h"
/*  A simple server in the internet domain using TCP
    The port number is passed as an argument
    http://www.linuxhowtos.org/C_C++/socket.htm
*/

ClientConnection startConfigurationClient(char *address, int portNumber){

    ClientConnection client;

    client.socketFileDescriptor = Socket(AF_INET, SOCK_STREAM, 0);

    client.server = GetHostByName(address);

    bzero((char *) &client.server_addr, sizeof(client.server_addr));
    client.server_addr.sin_family = AF_INET;
    bcopy((char *) client.server->h_addr, (char *) &client.server_addr.sin_addr.s_addr, client.server->h_length);
    client.server_addr.sin_port = htons(portNumber);

    Connect(client.socketFileDescriptor, (struct sockaddr *) &client.server_addr, sizeof(client.server_addr));

    return client;
}

int startClientConnection(char *address, int portNumber){
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
    numberCharReaded = read(client.socketFileDescriptor, buffer, 255);
    printf("Milliseconds message Recived: %lld\n", current_timestamp());
    if (numberCharReaded < 0) {
        error("ERROR reading from socket");
    }
    printf("%s\n",buffer);

    close(client.socketFileDescriptor);

    return 0;
}