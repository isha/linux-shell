#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_STRING_LENGTH 255
#define MAX_ARGUMENTS 100
#define DELIMITERS " \t"

//Splits the input string into segments depending on DELIMITERS, passes all parts into the array cmdArray, and returns the number of segments in the array.
int parseString(char* inputString, char*** cmdArray) {

  char* buffer;
  int numberOfCmds;

  buffer = (char*) malloc(strlen(inputString) * sizeof(char));
  strcpy(buffer,inputString);
  (*cmdArray) = (char**) malloc(MAX_ARGUMENTS * sizeof(char**));

  numberOfCmds = 0;
  (*cmdArray)[numberOfCmds++] = strtok(buffer, DELIMITERS);
  
  while ((((*cmdArray)[numberOfCmds] = strtok(NULL, DELIMITERS)) != NULL) && (numberOfCmds < MAX_ARGUMENTS)) {
	++numberOfCmds;
  }

  return numberOfCmds;
}

void prompt() {
	char* user = getenv( "USER" );
	char* system = malloc( 20 );
	char* path = getenv( "PWD" );
	gethostname( system, 20 );
	printf( "%s@%s: %s> ", user, system, path );
}

int main() {

  char inputString[MAX_STRING_LENGTH];
  char **cmdArray;
  int numberOfCmds;
  int count;
  

  while(1){
		prompt();
		gets(inputString);

		if (strcmp(inputString,"") != 0){
			
			numberOfCmds = parseString(inputString, &cmdArray);  
			
			//add an else if here for any accepted commands you are adding. See the current commands below for examples.
			if (strcmp(cmdArray[0],"exit") == 0){
				return 0;
			}
			else if (strcmp(cmdArray[0],"fork") == 0){
				int pid = fork();
				if(pid == 0){
					 printf("\nHello, this is child.");
					 return 0;
				}
			}
			
			//CD
			else if (strcmp(cmdArray[0],"cd") == 0){
				 if (numberOfCmds > 1){
					 if(setenv("PWD", cmdArray[1], 1) == 0){
					 }
					 else{
						printf("Invalid Path\n");
					 }
				 }
				 else{
				 }
			}
			
			//RUN
			else if (strcmp(cmdArray[0],"run") == 0){
				if (numberOfCmds > 1){
					int pid = fork();
					if(pid == 0){
						 system(strcat(strcat(getenv( "PWD" ),"/"), cmdArray[1]));
						 return 0;
					}
				}
			}
			
			else{
				printf("Unrecognised command: \"%s\"\n",cmdArray[0]);
			}
		}
  }
  
  return 0;

}
