#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues
void encrypt(char* one, char* two);

	char buffer[1000];

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	int childpid;
	socklen_t sizeOfClientInfo;
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

		// Enable the socket to begin listening
		if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
			error("ERROR on binding");
		listen(listenSocketFD, 5);
		
	int numClients=0;
	while(numClients < 5){
		// Accept a connection, blocking if not available until one connects
		// Get the size of the address for the client that will connect
		sizeOfClientInfo = sizeof(clientAddress); 
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");
		//printf("SERVER: Connected Client at port %d\n", ntohs(clientAddress.sin_port));
		numClients++; 
		int n; char* token1; char* token2; char*key; char* plain;
		char cipherText[200000];

		//fork off a child to handle the work
		childpid = fork();
		if (childpid ==0) {
		
			//printf("Child successfully created\n");
			close(listenSocketFD); // client process should not be listening

			while ( (n = recv(establishedConnectionFD, buffer, 1000, 0)) > 0)  {

  				//printf("String received %s\n", buffer);
				token1 = strtok(buffer, "#");
				if (strcmp(token1, "DEC") ==0){
					memset(buffer, '\0', sizeof(buffer));
					strcpy(buffer, "NO");
					puts(buffer);
   					send(establishedConnectionFD, buffer, n, 0);
					break;	
				}
				else {
				 	token1 = strtok(NULL, "#");
					//printf("the token1 is %s\n", token1);
					token2 = strtok(NULL, "#");
					//printf("the token2 is %s\n", token2);
					//printf("about to call the encrypt function\n");
					
					int numOne = 0, numTwo = 0, total = 0, j = 0; char* oneChar; char c;
					for (j=0; j < strlen(token1); j++){
						numOne = (int) token1[j]-65;
						//printf("%c (%d) + ", token1[j], numOne);
						//if statements handle the spaces
						if (numOne < 0)
							numOne = 26;
						numTwo = (int) token2[j]-65;
						//printf("%c (%d) = ", token2[j], numTwo);
						if (numTwo < 0)
							numTwo = 26;
						total = numOne + numTwo;
						//printf("total %d \n", total);
						if (total > 27){
							total = total - 27;
						}
						//printf("total %d\n", total);
						total += 65; //convert back to ascii
						//printf("the ascii total is %d\n", total);
						if (total > 90) {
							cipherText[j] = ' ';
						}
						else {
							cipherText[j] = total;
						}
						//printf("this was set to %c\n", cipherText[j]);
					}
					cipherText[j] = '\0';
					//memset(buffer, '\0', sizeof(buffer));
					strcpy(buffer, cipherText);
					//printf("buffer is %s\n", buffer);
					//puts(buffer);
   					send(establishedConnectionFD, buffer, n, 0);
					
				}
  			}

			if (n < 0)
   				printf("%s\n", "Read error");
  			exit(0);
		}

		// Close the existing socket which is connected to the client
		close(establishedConnectionFD);

		numClients--;
	}
	
	close(listenSocketFD); // Parent should close their listening socket
	return 0; 
}

void encrypt(char* one, char* two){
	
	printf("tok sub one is %c\n", one[0]);
	
	char* cipherText[strlen(one)+1];
	memset(cipherText, '\0', strlen(one)+1);
	
	//printf("this position holds %c\n", oneArray[0]);
	int numOne, numTwo;
	int j, total;
	for (j=0; j < strlen(one); j++){
		numOne = one[j]-65;
		printf("num1 %d\n", numOne);
		//if statements handle the spaces
		if (numOne < 0)
			numOne = 27;
		numTwo = two[j]-65;
		printf("num2 %d\n", numTwo);
		if (numTwo < 0)
			numTwo = 27;
		total = numOne + numTwo;
		total %=26;
		char c = (char) total;
		printf("the char is %c\n", c);
		//strcpy(cipherText[j], c);
	}
	//strcpy(cipherText[j], '\n');
	//strcpy(buffer, cipherText);

}
