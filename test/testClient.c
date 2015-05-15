#include "./../server/include/socketUtils.h"
#include "./../server/include/server.h"


int main(int argc, char *argv[]) {

    int portNumber = 4444;
    ServerConnection server = startConfigurationServer(portNumber);

    server.newSocketFileDescriptor[0] = Accept(server.socketFileDescriptor,
                                                  (struct sockaddr *) &server.cli_addr,
                                                  &server.clientLenght);
    printf("New connection , socket fd is %d , ip is : %s , port : %d \n",
           server.newSocketFileDescriptor[0], inet_ntoa(server.cli_addr.sin_addr),
           ntohs(server.cli_addr.sin_port));

    char *message = "1";
    //printf("%s",message);

    Send(server.newSocketFileDescriptor[0], message, strlen(message));
    sleep(1);

    long long timeToStart = timeToStartInSeconds(2);

    printf("Sending timeToStart %lld, IDClient: %d \n",timeToStart,1);

    sendData(server.newSocketFileDescriptor[0],timeToStart,1);

    sleep(20);

    printf("Changing Client from 1 to 2\n");

    sendData(server.newSocketFileDescriptor[0],0,2);


    sleep(10);

    printf("Changing Client from 2 to 1\n");

    sendData(server.newSocketFileDescriptor[0],0,1);

    sleep(10);

    printf("Exit!\n");

    sendData(server.newSocketFileDescriptor[0],0,-1);

    return 0;

}