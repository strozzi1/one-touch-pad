#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues


int main(int argv, char *argc[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	char* inputfile;
	char* keyfile;
	inputfile = argc[1];
	keyfile = argc[2];

	//printf("input: %s | keyfile: %s\n", inputfile, keyfile);

	//get files
	FILE * fd1, *fd2;														//my file pointers

	fd1 = fopen(inputfile, "r");								//open input file
	if(!fd1){
		perror("fd1 fopen() was an utter failure\n");
		fflush(stdout);
		exit(1);
	}

	fd2 = fopen(keyfile, "r");									//open keyfile
  if(!fd2){
    perror("fd2 open() was an utter failure");
    fflush(stdout);
    return 1;
  }

	fseek(fd1, 0, SEEK_END);						//go to end of file
	int inputlen = ftell(fd1);					//get file length
	fseek(fd2, 0, SEEK_END);						//repeat for key file
  int keylen = ftell(fd2);

	if(inputlen > keylen){							//compare size of input to size of key
    perror("Key isn't big enough for the input file\n");
    fflush(stdout);
    return 1;
  }
  //reset file pointer to beginning
	fseek(fd1, 0, SEEK_SET);
  fseek(fd2, 0, SEEK_SET);

	char* inbuff = malloc(inputlen*sizeof(char));			//set up text array
	memset(inbuff, '\0', inputlen*sizeof(char));			//clear it

	if(fgets(inbuff, inputlen, fd1)!=NULL){						//fill array with the stuff

		inbuff[inputlen-1]='\0';
	}
	fclose(fd1);


	char* keybuff = malloc(keylen*sizeof(char));			//repeat for the key
  if(fgets(keybuff, keylen, fd2)!=NULL){

    keybuff[keylen-1]='\0';													//instead of last character being a newline, it's a null terminator
  }
  fclose(fd2);
	//printf("input: %s", inbuff);
	//now concat those and send it to the servers
	int size =  (sizeof(char)*inputlen) + ( sizeof(char) * keylen) + (5*sizeof(char)) ;
	char* output = malloc(size);
	memset(output, '\0', size );

//	printf("sizeof array: %d\n", size);
	sprintf(output, "e#%s@%s$", inbuff, keybuff);				//cat together my symbols, message, and key
	//set up output to server
	//printf("catted: %s\n",output);
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[256];

	if (argv < 4) { fprintf(stderr,"USAGE: %s hostname port\n", argc[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argc[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address					//instead of using a cmd arg for "localhost", just set it by name
	//serverHostInfo = gethostbyname(argc[1]); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

/*
	// Get input message from user  REMOVE
	printf("CLIENT: Enter text to send to the server, and then hit enter: ");
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
	buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds
*/

	//from here on out, edit this!

	/*// Send message to server
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server					IMPORTANTE!!!!!
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	*/
	int totalsent=0, lastsent =0;
	char* outptr = output;
	while (totalsent<strlen(output) ){
		charsWritten = send(socketFD, outptr+totalsent*sizeof(char), strlen(output)-totalsent, 0); // Write to the server					IMPORTANTE!!!!!
		totalsent+=charsWritten;
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
		if (charsWritten < strlen(output)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	}





	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

	close(socketFD); // Close the socket
	return 0;
}
