#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 

#include "netio.h"
#include "writeFile.h"

#define SERVER_PORT 5678
#define MAX_PENDING 10

void keepAlive(int socketfd, char *info_hash) {
    char *filePath = getFilePath();
    int pieceLength = getPieceLength();
    char *pieces = getPieces();
    int fileLength = getFileLength();
    int iterationsPerPiece = pieceLength/65536;
    int len = 0, id = 0;
    //Would be better if we were able to get all incomplete pieces
    char *requested_piece = getIncompletePiece(filePath);
    int pieceIndex;
    int pieceBegin;
    int pieceBlock;
    int blockLength;
    //Sees if there is any available message.
    //If there is, it's a request for a piece.
    //If not, we can request our piece.
    while(1) {
        if( recv(socketfd,&len,4,0) != EWOULDBLOCK) {
            while(len == 0x0013) {
                //Someone is requestion a block
                //Get all parameters and see if we have block
                //If we have block, return the block
                //Otherwise, return cancel with initial params
                recv(socketfd,&id,4,0);
                recv(socketfd,&pieceIndex,4,0);
                recv(socketfd,&pieceBegin,4,0);
                recv(socketfd,&blockLength,4,0);
                pieceBlock = (char *)calloc(blockLength,sizeof(char));
                if(verifyIfHasPiece(filePath,pieceLength,pieceIndex,getIntialSHA(pieces,pieceLength,pieceIndex)) == 1) {
                    //Send files
                } else {
                    //Send cancel
                }
            }
        }
        pieceIndex = getPieceIndex(requested_piece);
        while(requested_piece != NULL) {
            len = 0x0013;
            id = 0x6;
            for(int i=0; i<=iterationsPerPiece;i++) {
                send(socketfd,len,4,0);
                send(socketfd,id,1,0);
                send(socketfd,pieceIndex,4,0);
                send(socketfd,i*iterationsPerPiece,4,0);
                send(socketfd,65536,4,0);
            }
            recv(socketfd,&len,4,0);
            while(len != 0x0013) {
                len = len - 0x0009;
                recv(socketfd,id,4,0);
                if(id != 0x7)
                    return;
                recv(socketfd,&pieceIndex,4,0);
                recv(socketfd,&pieceBegin,4,0);
                recv(socketfd,&pieceBlock,len,0);
                writeToFile(filePath,len,pieceIndex+pieceBegin,pieceBlock);
                recv(socketfd,&len,4,0);
            }
            if(len == 0x0009)
                break;
            requested_piece = getIncompletePiece(filePath);
        }
    }
}

char *waitForHandshake(int socketfd) {
    int pstrlen;
    char *reserved = (char *)calloc(8,sizeof(char));
    char *info_hash = (char *)calloc(20,sizeof(char));
    char *peerID = (char *)calloc(20,sizeof(char));
    recv(socketfd,&pstrlen,4,0);
    char *pstr = (char *)calloc(pstrlen,sizeof(char));
    recv(socketfd,pstr,pstrlen,0);
    recv(socketfd,reserved,8,0);
    recv(socketfd,info_hash,20,0);
    recv(socketfd,peerID,20,0);
    if(haveInfoHash(info_hash) == 0) {
        close(socketfd);
        return NULL;
    }
    send(socketfd,&pstrlen,4,0);
    send(socketfd,pstr,pstrlen,0);
    send(socketfd,reserved,8,0);
    send(socketfd,info_hash,20,0);
    send(socketfd,peerID,20,0);
    free(reserved);
    free(peerID);
    free(pstr);
    return info_hash;
}

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
            char *response = waitForHandshake(newsockfd);
            if(response != NULL) {
                keepAlive(newsockfd,response);
            }
            
            exit(0);
        }
         
        //parinte
        close(newsockfd);

   
    exit(0);
    }
}