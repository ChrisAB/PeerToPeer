#ifndef METAINFO_H
#define METAINFO_H
#include "bencoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Metainfo Metainfo;
typedef struct infoDictionary infoDictionary;

Bencoding *getMetainfoFromFile(FILE *f) {
    //Dictionary
    return readBeconding(f);
}

int getPieceLength(char *filePath)
{

	FILE *f = fopen(filePath, "r");
	Bencoding *dictionary = getMetainfoFromFile(f);
	dictNode *dict = dictionary->value.dict;
	while(1)
	{
		if(strcmp(dict->key , "info")==0){
			
			dictNode *infoD = dict->value->value.dict;
			
			while(1)
			{
				
				if(strcmp(infoD->key , "piecelength")==0){
					
					return (int) infoD->value->value.val;
					
				}
				
				infoD = infoD->next;
			}
			
		}
			
		dict = dict -> next;
	}
	
}
	
char *getPieces(char *filePath)
{
	FILE *f = fopen(filePath, "r");
	Bencoding *dictionary = getMetainfoFromFile(f);
	dictNode *dict = dictionary->value.dict;
	while(1)
	{
		if(strcmp(dict->key , "info")==0){
			
			dictNode *infoD = dict->value->value.dict;
			
			while(1)
			{
				
				if(strcmp(infoD->key , "pieces")==0){
					
					return infoD->value->value.str;
					
				}
				
				
				infoD = infoD ->next;
			}
			
		}
			
		dict = dict -> next;
	}
	
}
	
int getFileLength(char *filePath)
{

	FILE *f = fopen(filePath, "r");
	Bencoding *dictionary = getMetainfoFromFile(f);
	dictNode *dict = dictionary->value.dict;
	while(1)
	{
		if(strcmp(dict->key , "info")==0){
			
			dictNode *infoD = dict->value->value.dict;
			
			while(1)
			{
				
				if(strcmp(infoD->key , "length")==0){
					
					return (int) infoD->value->value.val;
					
				}
				
				
				infoD = infoD ->next;
			}
			
		}
			
		dict = dict->next;
	}
	
}

int getIncompletePiece(char *pieces, int pieceLength, int fileLength, char *filePath)
{
	
	//int verifyIfHasPiece(char *filePath, int pieceLength, int pieceIndex, char *initialSHA)

	int pieceNr = strlen(pieces)/pieceLength;

	char init[21];

	for(int i=0;i<pieceNr;i++)
	{
		
		memcpy(init, pieces+(20*i), 20);
		init[20] = '\0';
		
		if(verifyIfHasPiece(filePath,pieceLength,i,init)==-1)
			return pieceLength*i;
		
	}
	
	return 0;

}
#endif