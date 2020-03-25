#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 

#include "netio.h"

#define SERVER_PORT 5678
#define MAX_PENDING 10

// CREATE A SOCKET
int main(void){

    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t cLen;

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
    		perror("Error at socket opening");
    		exit(1);
    }
    set_addr(&serv_addr, NULL, INADDR_ANY, SERVER_PORT)	;

    //ASSIGN A PORT TO SOCKET
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
    		perror("Error: Binding Failed"); 
    		close(sockfd);
    		exit(1);
    }

    //LISTEN
    if (listen(sockfs, MAX_PENDING) < 0){
        perror("Error: Binding Failed"); 
        exit(1);
    }

    //ACCEPT CONNECTIONS
    for(;;){

        //accept new connection
        cLen = sizeof(client_addr);
        newsockfd = acccept(sockfd, (struct sockaddr *)&client_addr, &cLen);
        if (newsockfd < 0) {
            perror ("Error: Can't accept req"); 
            exit(1);
         }

         if (fork() == 0){ //prunc 
             
            close(sockfd);
            /* cod aka funtie care trebe sa faca chestia pe care inca
                habar nu am cum sa o fac */
            exit(0)
        }
         
        //parinte
        close(newsockfd);

   
    exit(0);
}