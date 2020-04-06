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

char *filePath;

void keepAlive(int socketfd, char *info_hash) {
    const int blockSizeMax = 65536;
    int pieceLength = getPieceLength(filePath);
    char *pieces = getPieces(filePath);
    int fileLength = getFileLength(filePath);
    int iterationsPerPiece = pieceLength/65536;
    int len = 0, id = 0;
    //Would be better if we were able to get all incomplete pieces
    int requested_piece = getIncompletePiece(pieces,pieceLength,fileLength, filePath);
    int pieceIndex;
    int pieceBegin;
    char *pieceBlock;
    int blockLength;
    //Sees if there is any available message.
    //If there is, it's a request for a piece.
    //If not, we can request our piece.
    while(1) {
        if( recv(socketfd,&len,4,0) != EWOULDBLOCK) {
            while(len == 0x0013) {
                //Someone is requestion a block
                //Get all parameters and see if we have block
                recv(socketfd,&id,4,0);
                recv(socketfd,&pieceIndex,4,0);
                recv(socketfd,&pieceBegin,4,0);
                recv(socketfd,&blockLength,4,0);
                //If we have block, return the block
                if(verifyIfHasPiece(filePath,pieceLength,pieceIndex,getIntialSHA(pieces,pieceLength,pieceIndex)) == 1) {
                    len = 0x0009 + blockLength;
                    id = 0x7;
                    pieceBlock = (char *)calloc(blockLength,sizeof(char));
                    pieceBlock = readFromFile(filePath,blockLength,pieceIndex+pieceBegin);
                    send(socketfd,&len,4,0);
                    send(socketfd,&id,4,0);
                    send(socketfd,&pieceIndex,4,0);
                    send(socketfd,&pieceBegin,4,0);
                    send(socketfd,&pieceBlock,blockLength,0);
                //Otherwise, return cancel with initial params
                } else {
                    send(socketfd,&len,4,0);
                    send(socketfd,&id,4,0);
                    send(socketfd,&pieceIndex,4,0);
                    send(socketfd,&pieceBegin,4,0);
                    send(socketfd,&blockLength,4,0);
                    break;
                }
                recv(socketfd,&len,4,0);
            }
        }
        while(requested_piece != 0) {
            len = 0x0013;
            id = 0x6;
            for(int i=0; i<=iterationsPerPiece;i++) {
                send(socketfd,&len,4,0);
                send(socketfd,&id,1,0);
                send(socketfd,&pieceIndex,4,0);
                pieceBegin = i*iterationsPerPiece;
                send(socketfd,&pieceBegin,4,0);
                send(socketfd,&blockSizeMax,4,0);
            }
            recv(socketfd,&len,4,0);
            while(len != 0x0013) {
                len = len - 0x0009;
                recv(socketfd,&id,4,0);
                if(id != 0x7)
                    return;
                recv(socketfd,&pieceIndex,4,0);
                recv(socketfd,&pieceBegin,4,0);
                pieceBlock = (char *)calloc(len,sizeof(char));
                recv(socketfd,&pieceBlock,len,0);
                writeToFile(filePath,len,pieceIndex+pieceBegin,pieceBlock);
                recv(socketfd,&len,4,0);
            }
            if(len == 0x0013)
                break;
            requested_piece = getIncompletePiece(pieces, pieceLength, pieceIndex, filePath);
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

void sendHandshake(int socketfd) {
    int pstrlen = 19;
    char *pstr = bencodeString("BitTorrent protocol");
    char *reserved = (char *)calloc(8,sizeof(char));
    sprintf(reserved, "00000000");
    char *info_hash = getInfoHash();
    char *peerID = "DP123456789123456789";
    send(socketfd,&pstrlen,4,0);
    send(socketfd,pstr,pstrlen,0);
    send(socketfd,reserved,8,0);
    send(socketfd,info_hash,20,0);
    send(socketfd,peerID,20,0);
    int pstrlenR;
    char *reservedR = (char *)calloc(8,sizeof(char));
    char *info_hashR = (char *)calloc(20,sizeof(char));
    char *peerIDR = (char *)calloc(20,sizeof(char));
    recv(socketfd,&pstrlenR,4,0);
    char *pstrR = (char *)calloc(pstrlen,sizeof(char));
    recv(socketfd,pstrR,pstrlen,0);
    recv(socketfd,reservedR,8,0);
    recv(socketfd,info_hashR,20,0);
    recv(socketfd,peerIDR,20,0);
}

// CREATE A SOCKET
int main(int argc, char **argv){
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, client_addr, remote_addr;
    socklen_t cLen;
    if(argc != 4) {
        fprintf(stderr,"Format: ./a.exe host/connect filename IP");
    }

    filePath = argv[2];

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
    if(strcmp("host",argv[1]) == 0) {
        if (listen(sockfd, MAX_PENDING) < 0){
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
    } else {
        set_addr(&remote_addr,  argv[3], 0, SERVER_PORT);

        //establish connection
        if((connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(remote_addr)))< 0){
            perror("Error: Connection to server Failed"); 
            close(sockfd);
            exit(1);
        }
        char *info_hash = sendHandshake(sockfd);
        keepAlive(sockfd, info_hash);
    }
}