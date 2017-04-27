/*
Author: Sana Tauqir
Project: CS 344 Project 4
*/

#include <stdlib.h>
#include <stdio.h>


int main(int argc, char* argv[]) {

	//variables
	int length, j, r;
	time_t t;

	//parameter checking
	if (argc < 2 || argc > 2){
		perror("Usage: keygen <length> ");
		exit(1);
	}

	length = atoi(argv[1]);

	//seed random generator
	srand((unsigned) time(&t));

	for (j=0; j < length; j++){
		r = rand() % 27;
		char c = r+65;
		//include spaces
		if (c == 91){
			printf(" ");}
		else {
			printf("%c", c);}
	}
	
	printf("\n");
	return 0;
}
