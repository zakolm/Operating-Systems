#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h> //socket, bind

#include "info.h"

int sockfd;


int main(void)
{
    char msg[MSG_LEN]; 
    struct sockaddr client_addr;

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Error in socket(): ");
        return sockfd;
    }

    client_addr.sa_family = AF_UNIX;
    strcpy(client_addr.sa_data, SOCKET_NAME);

    if (bind(sockfd, &client_addr, sizeof(client_addr)) < 0)
    {
        printf("Closing socket....\n");
        close(sockfd);
        unlink(SOCKET_NAME);
        perror("Error in bind(): ");
        return -1;
    }

    printf("\nServer is waiting for the message...\n");

    for(;;)
    {

        int recievedSize = recv(sockfd, msg, sizeof(msg), 0);
        if (recievedSize < 0) 
        {
            close(sockfd);
            unlink(SOCKET_NAME);
            perror("Error in recv(): ");
            return recievedSize;
        }

        msg[recievedSize] = 0;
        printf("Client send this message: %s\n", msg);
    }
    
    printf("Closing socket....\n");
    close(sockfd);
    unlink(SOCKET_NAME);
    return 0;
}
