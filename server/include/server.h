#ifndef CONNECTION_SERVER
#define CONNECTION_SERVER

#include "./funcions.h"
#include "./socketUtils.h"

typedef struct ServerConnectionTag{
    /* *********************************************************************
    * socketFileDescriptor and newSocketFileDescriptor are file descriptors,
    * i.e. array subscripts into the file descriptor table .
    * These two variables store the values returned by the socket system call
    * and the accept system call.
    * ********************************************************************** */
    int socketFileDescriptor;
    int newSocketFileDescriptor[30];

    /***********************************************************************
    * portNumber stores the port number on which the server accepts connections.
    * ********************************************************************** */
    int portNumber;

    /***********************************************************************
    * clientLenght stores the size of the address of the client.
    * This is needed for the accept system call.
    * ********************************************************************** */
    socklen_t clientLenght;

    /***********************************************************************
    * A sockaddr_in is a structure containing an internet address.
    * This structure is defined in netinet/in.h.
    *
    * The variable serv_addr will contain the address of the server, and
    * cli_addr will contain the address of the client which connects to the server.
    * ********************************************************************** */
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

    int max_clients;

} ServerConnection;
/*
* To bee able to use ServerConnection everywhere you want to avoid adding the
* struct keyword, and still use ServerConnectionTag when a typedef won't work.
*/

void  writeTimeDelay(long long delayTime);
void sendData(int sock, long long timeToStart, int idCli);

ServerConnection startConfigurationServer(int portNumber);
int startServerConnection(int portNumber);
void notifyClients(ServerConnection server, long long timeToStart, int IDCli);

#endif