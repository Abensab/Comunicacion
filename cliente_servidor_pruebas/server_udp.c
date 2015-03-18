/* A simple server in the internet domain using TCP
   The port number is passed as an argument
   http://www.linuxhowtos.org/C_C++/socket.htm
   */
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sock, length, n,portno;
    socklen_t fromlen;
    struct sockaddr_in server;
    struct sockaddr_in from;
    char buf[1024];

    if (argc < 2) {
      fprintf(stderr, "ERROR, no port provided\n");
      exit(0);
    }

    sock=socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {
        error("ERROR opening socket");
    }

    length = sizeof(server);
    bzero(&server, length);

    portno = atoi(argv[1]);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(portno);

    if (bind(sock,(struct sockaddr *)&server,length)<0){
       error("binding");
    }

    fromlen = sizeof(struct sockaddr_in);

    while (1) {
        n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *)&from, &fromlen);
        if (n < 0){
           error("recvfrom");
        }else{
            printf("Conection made from: %d.%d.%d.%d\n",
                    from.sin_addr.s_addr&0xFF,
                    (from.sin_addr.s_addr&0xFF00)>>8,
                    (from.sin_addr.s_addr&0xFF0000)>>16,
                    (from.sin_addr.s_addr&0xFF000000)>>24);
        }

        write(1, "Received a datagram: ",21);
        write(1, buf, n);

        n = sendto(sock,"Got your message\n", 17, 0,(struct sockaddr *)&from,fromlen);
        if (n  < 0){
           error("sendto");
        }
    }
    return 0;
}
