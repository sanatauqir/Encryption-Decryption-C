#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues
void concatFiles(char* buffer, char* wholeMessage, char* plainName, char*keyName);

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
    	struct stat keySize, plainSize;
	//argument validation
	if (argc < 3) { fprintf(stderr,"USAGE: %s plaintext keyfile port\n", argv[0]); exit(0); }

	//badchars and keyfile size check
	stat(argv[2], &keySize);
	stat(argv[1], &plainSize);
	if (keySize.st_size < plainSize.st_size){
		fprintf(stderr, "key is too short\n");
		exit(1);
	}

	int size= (plainSize.st_size*2) + 10;
	char buffer[size]; char wholeMessage[size];

	//printf("a buffer was created of size %d\n", size);

	/**************SETUP (will move into function later)*********************/
	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost");
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(2); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");
	/******************END OF SETUP*****************/

	// Send the filetype, plaintext file and keyfile to the server
	memset(buffer, '\0', sizeof(buffer));
	concatFiles(buffer, wholeMessage, argv[1], argv[2]);
	//printf("returned from concatFiles all right \n");

	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer))
	       	printf("CLIENT: WARNING: Not all data written to socket!\n");
	//printf("CLIENT: %d\n", charsWritten);

	// When otp_enc receives the ciphertext back from otp_enc_d
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer)-1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");

	if (strcmp(buffer, "NO") == 0){
		fprintf(stderr, "Not an encryption server");
		exit(1);
	}
	//strcat(buffer[size], '\n');
	//it should ouput it to stdout
	printf("%s\n", buffer);

	close(socketFD); // Close the socket

	exit(0);
	return 0;
}

void concatFiles(char* buffer, char* wholeMessage, char* plainName, char* keyName){
	ssize_t nread; char c;
	char* oneChar;
	struct stat keySize, plainSize;
	stat(plainName, &plainSize);
	char fileBuffer[plainSize.st_size];
	memset(wholeMessage, '\0', sizeof(wholeMessage));
	memset(fileBuffer, '\0', sizeof(fileBuffer));

	//declare yourself
	strcat(wholeMessage, "ENC#");

	int file_descriptor = open(plainName, O_RDONLY, 0666);
	if (file_descriptor < 0)
		printf("error opening file\n");
	lseek(file_descriptor, 0, SEEK_SET);
	nread = read(file_descriptor, fileBuffer, sizeof(fileBuffer));
	if (nread < 0)
		printf("not reading the file properly\n");
	int i, numChar;
	for (i = 0; i < (strlen(fileBuffer)-2); i++) {
		//printf("char is %c\n", fileBuffer[i]);
		numChar = (int) fileBuffer[i];
		if (((numChar < 65) || (numChar > 90)) && (numChar != 32)) {
			fprintf(stderr, "Invalid characters in %s\n", plainName);
			exit(1);
			
		}
		sprintf(oneChar, "%c",fileBuffer[i]);
		strcat(wholeMessage, oneChar);
	}
	close(file_descriptor);
	strcat(wholeMessage, "#");

	memset(fileBuffer, '\0', sizeof(fileBuffer));
	file_descriptor = open(keyName, O_RDONLY,0666);
	if (file_descriptor < 0)
		printf("error opening file\n");
	lseek(file_descriptor, 0, SEEK_SET);
	nread = read(file_descriptor, fileBuffer, sizeof(fileBuffer));
	if (nread < 0)
		printf("not reading the file properly\n");
	i = 0; numChar = 0;
	for (i = 0; i < (strlen(fileBuffer) - 2); i++) {
		//printf("char is %c\n", fileBuffer[i]);
		numChar = (int) fileBuffer[i];
		//printf("numchar is %d\n", numChar);
		if (((numChar < 65) || (numChar > 90)) && (numChar != 32)) {
			fprintf(stderr, "Invalid characters in %s\n", keyName);
			exit(1);
		}
		sprintf(oneChar, "%c",fileBuffer[i]);
		strcat(wholeMessage, oneChar);
	}
	close(file_descriptor);
	strcat(wholeMessage, "#");

	strcpy(buffer, wholeMessage);
}
