#ifndef METAINFO_H
#define METAINFO_H
#include "bencoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Metainfo Metainfo;
typedef struct infoDictionary infoDictionary;

Bencoding *getMetainfoFromFile(FILE *f) {
    Bencoding *curr;
    //Dictionary
    return readBeconding(f);
}

char *getPieces(char *filePath) {
    FILE *f = fopen(filePath,"r");
    Bencoding *bigDict = getMetainfoFromFile(f);
    dictNode *dict = bigDict->value.dict;
    while(1) {
        if(strcmp("info",dict->key) == 0) {
            dict = dict->value->value.dict;
            while(1) {
                if(strcmp("pieces",dict->key) == 0) {
                    fclose(f);
                    return dict->value->value.str;
                }
                dict = dict->next;
            }
        }
        dict = dict->next;
    }
}

char *getPieceLength(char *filePath) {
    FILE *f = fopen(filePath,"r");
    Bencoding *bigDict = getMetainfoFromFile(f);
    dictNode *dict = bigDict->value.dict;
    while(1) {
        if(strcmp("info",dict->key) == 0) {
            dict = dict->value->value.dict;
            while(1) {
                if(strcmp("piece length",dict->key) == 0) {
                    fclose(f);
                    return dict->value->value.val;
                }
                dict = dict->next;
            }
        }
        dict = dict->next;
    }
}

char *getFileLength(char *filePath) {
    FILE *f = fopen(filePath,"r");
    Bencoding *bigDict = getMetainfoFromFile(f);
    dictNode *dict = bigDict->value.dict;
    while(1) {
        if(strcmp("info",dict->key) == 0) {
            dict = dict->value->value.dict;
            while(1) {
                if(strcmp("length",dict->key) == 0) {
                    fclose(f);
                    return dict->value->value.val;
                }
                dict = dict->next;
            }
        }
        dict = dict->next;
    }
}

#endif