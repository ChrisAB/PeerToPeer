#include <stdio.h>
#include <openssl/sha.h>

//Creates file with bytes size
//This can be implemented in more than 1 way
void preallocateFile(char *filePath, long long bytes) {
	
	FILE *file;
	file = fopen(filePath, "w");
	fseek(file, bytes-1, SEEK_SET);
	fputc('1', file);
	fputc('\0', file);
	fclose(file);
}

//Writes pieceLength bytes to filePath beginning from pieceIndex.
void writeToFile(char *filePath, int pieceLength, int pieceIndex, char *bytes) {
	
	FILE *file;
	file = fopen(filePath, "w");

	if (file == NULL)
	{
		printf("File write error!");
		exit(1);
	}

	fseek(file, pieceIndex, SEEK_SET);
	fwrite(bytes, pieceLength, sizeof(char), file);

	fclose(file);
}

//Reads pieceLength bytes from file starting at pieceIndex then returns that as char *
char *readFromFile(char *filePath, int pieceLength, int pieceIndex) {
	FILE *file;
	file = fopen(filePath, "r");

	if (file == NULL)
	{
		printf("File write error!");
		exit(1);
	}

	fseek(file, pieceIndex, SEEK_SET);
	char *bytes = (char*)malloc(pieceLength);
	fread(bytes, pieceLength, sizeof(char), filePath);
	
	return bytes;
}

//Reads the piece at pieceIndex from filePath, transforms it into SHA1 using openssl/sha.h
//Compare computed SHA1 to initialSHA and return 1-ok, otherwise
//Create error code as you wish.
int verifyIntegrity(char *filePath, int pieceLength, int pieceIndex, char* initialSHA) {
	
	unsigned char hash[SHA_DIGEST_LENGTH];
	char *bytes = readFromFile(filePath, pieceLength, pieceIndex);

	if (strcmp(initialSHA,SHA1(bytes, sizeof(bytes), hash))){
		return 1;
	}
	return 0;
}


//Verify if the data at pieceIndex is equal to initialSHA.
//If it is, it means you have that piece. Otherwise, you don't.
int verifyIfHasPiece(char *filePath, int pieceLength, int pieceIndex, char *initialSHA) {
    return verifyIntegrity(filePath, pieceLength, pieceIndex, initialSHA) == 1 ? 1 : -1;
}