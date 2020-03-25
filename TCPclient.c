#include <stdlib.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <fcntl.h>

#include "netio.h"

#define SERVER_ADDRESS '127.0.0.1'
#define SERVER_PORT 5678

int main(int argc, char *argv[]){
    int sockfd;
    struct sockaddr_in local_addr, remote_addr;

    //check arguments if any 

     if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
    		perror("Error at socket opening");
    		exit(1);
    }

    set_addr(&local_addr, NULL, INADDR_ANY, 0);

    if (bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0){
    		perror("Error: Binding Failed"); 
    		close(sockfd);
    		exit(1);
    }

     set_addr(&remote_addr,  SERVER_ADDRESS, 0, SERVER_PORT);

    //establish connection
    if((connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(remote_addr)))< 0){
        perror("Error: Connection to server Failed"); 
    	close(sockfd);
    	exit(1);
    }

    /* 
    cod 
    */
    close(sockfd);
    close(fd);
}