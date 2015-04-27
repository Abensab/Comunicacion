#ifndef _INCLUDE_SOCKUTILS_H
#define _INCLUDE_SOCKUTILS_H

/**STANDARD LIBRARIES**/
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <netdb.h>

#include <stdlib.h> // getenv()
#include <sys/types.h>
#include <sys/time.h> // timeval
#include <sys/select.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>     /* for close() */
#include <signal.h>

#include <sys/wait.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>     /* for atoi() and exit() */

/**CONSTANTS DEFINE**/
#define FAILURE 1
#define SUCCESS 0

#define SA struct sockaddr

int Socket (int family, int type, int protocol);
int Bind (int sockfd, const SA *myaddr, socklen_t myaddrlen);
int Listen (int sockfd, int backlog);
int Accept (int listen_sockfd, SA *cliaddr, socklen_t *addrlenp);
int Connect (int sockfd, const SA *srvaddr, socklen_t addrlen);
struct hostent *GetHostByName(char *host);
void Close (int fd);
int Select(int maxfdp1,fd_set *readset,fd_set *writeset,fd_set *exceptset,struct timeval *timeout);
int Recv(int socket,void *buffer,int len,int flag);
int Send(int socket,void *buffer,int len);

#endif