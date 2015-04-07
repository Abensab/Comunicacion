#ifndef CONNECTION
#define CONNECTION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <arpa/inet.h>

/*Kbit arate*/
#include "spatiallib.h"
#include <termios.h>
#include <fcntl.h>

typedef struct ServerConnectionTag{
    /* *********************************************************************
    * socketFileDescriptor and newSocketFileDescriptor are file descriptors,
    * i.e. array subscripts into the file descriptor table .
    * These two variables store the values returned by the socket system call
    * and the accept system call.
    * ********************************************************************** */
    int socketFileDescriptor;

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


} ServerConnection;
/*
* To bee able to use ServerConnection everywhere you want to avoid adding the
* struct keyword, and still use ServerConnectionTag when a typedef won't work.
*/


typedef struct ClientConnectionTag{
    /* *********************************************************************
    * socketFileDescriptor are file descriptors,
    * i.e. array subscripts into the file descriptor table .
    * These two variables store the values returned by the socket system call
    * and the accept system call.
    * ********************************************************************** */
    int socketFileDescriptor;

    /***********************************************************************
    * portNumber stores the port number on which the server accepts connections.
    * ********************************************************************** */
    int portNumber;

    /***********************************************************************
    * A sockaddr_in is a structure containing an internet address.
    * This structure is defined in netinet/in.h.
    *
    * The variable serv_addr will contain the address of the server, and
    * cli_addr will contain the address of the client which connects to the server.
    * ********************************************************************** */
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;

    /***********************************************************************
    * The variable server is a pointer to a structure of type hostent.
    * This structure is defined in the header file netdb.h.
    * ********************************************************************** */
    struct hostent *server;

    unsigned int length;

 } ClientConnection;


    long long current_timestamp();
    void error(const char *msg);
    int playSuperWav();
    int kbhit(void);
    void  writeTimeDelay(long long delayTime);
    void dostuff (ServerConnection server);

    int startServerConnection(int portNumber);
    ServerConnection startConfigurationServer(int portNumber);

    int startClientConnection(char *address, int portNumber);
    ClientConnection startConfigurationClient(char *address, int portNumber);
#endif