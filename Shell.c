#include <sys/stat.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
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
	char* user = malloc( strlen(getenv("USER"))*sizeof(char) );
	char* system = malloc( MAX_STRING_LENGTH * sizeof(char) );
	char* home = malloc( MAX_STRING_LENGTH * sizeof(char) );
	char* path = malloc( strlen(getenv("PWD"))*sizeof(char) );
	strcpy(user, getenv("USER"));
	strcpy(path, getenv("PWD"));
	gethostname( system, 20 );
	sprintf( home, "/home/%s", user );
	if( strncmp( home, path, strlen(home) ) == 0 ) {
		memmove( path, ( path + strlen( home ) - 1 ), ( strlen(path) - strlen(home) + 1 ) );
		path[0] = '~';
		path[strlen(path) - strlen(home) + 1] = '\0';
	}
	printf( "%s@%s:%s$ ", user, system, path );
}

int main() {

  char inputString[MAX_STRING_LENGTH];
  char **cmdArray;
  int numberOfCmds;
  int count;
  extern  int alphasort();
  

  while(1){
	  
		prompt();
		gets(inputString);

		if (strcmp(inputString,"") != 0){
			
			numberOfCmds = parseString(inputString, &cmdArray);  
			
			if (strcmp(cmdArray[0],"exit") == 0){
				return 0;
			}
			
			/*	CHANGE PATH VARIABLE
			    Append with / to start new directory or without to add to current directory.
				usage: cd [directory] 
			*/
			else if (strcmp(cmdArray[0],"cd") == 0){
				 if (numberOfCmds > 1){
					 if(strncmp(cmdArray[1], "/", 1) == 0){
						 if(setenv("PWD", cmdArray[1], 1) == 0){
						 }
						 else{
							printf("Invalid Path\n");
						 }
					 } else {
						 strcat(strcat(getenv( "PWD" ), "/"), cmdArray[1]);
					 }
					 
				 } else{
					 printf("Not enough arguments.\n");
				 }
			}
			
			/*	RUN PROGRAM
				usage: run [name] 
			*/
			else if (strcmp(cmdArray[0],"run") == 0){
				if (numberOfCmds > 1){
					int pid = fork();
					if(pid == 0){
						system(strcat(strcat(getenv( "PWD" ),"/"), cmdArray[1]));
						return 0;
					}
				} else {
					printf("Not enough arguments.\n");
				}
			}

			/*	SET ENVIRONMENT VARIABLE
				usage: setenv [name] [value]
			*/
			else if (strcmp(cmdArray[0],"setenv") == 0){
				if( numberOfCmds > 2 ){
					if( setenv( cmdArray[1], cmdArray[2], 1 ) != 0 ) printf("Failed to set environment variable.\n");
				} else {
					printf("Not enough arguments.\n");
				}
			}

			/*	GET ENVIRONMENT VARIABLE
				usage: getenv [name]
			*/
			else if (strcmp(cmdArray[0],"getenv") == 0){
				if( numberOfCmds > 1 ){
					if( getenv( cmdArray[1] ) != NULL ) printf( "%s : %s\n", cmdArray[1], getenv( cmdArray[1] ) );
					else printf( "Environment variable %s does not exist.\n", cmdArray[1] );
				} else {
					printf("Not enough arguments.\n");
				}
			}

			/*	Make Directory
				usage: mkdir [directoryName]
				options:
					-p : all nonexisted directories specified are created
					-m : allow for permissions to be specified
					-v : prints out all directories created
			*/
			else if (strcmp(cmdArray[0],"mkdir") == 0){
				if( numberOfCmds > 1 ){
					//TODO: Add optional functionality.
					int i;
					for( i = 1; i < numberOfCmds; i++ ) {
						if( strncmp(cmdArray[i], "-", 1) == 0 ) {
							switch( cmdArray[i][1] ) {
								case 'p':
								break;
								case 'v':
								break;
								case 'm':
								break;
								default:
									printf("mkdir: invalid option -- \'%c\'", cmdArray[i][1] );
								}
						} else {
							if( mkdir( cmdArray[i], S_IRWXU|S_IRGRP|S_IXGRP ) != 0 ) puts("mkdir: could not create directory.");
						}
					}
				} else {
					printf("Not enough arguments.\n");
				}
			}
			
			/*	LIST ALL FILES IN DIRECTORY
				usage: dir 
			*/
			else if (strcmp(cmdArray[0],"dir") == 0){
				
				struct dirent **namelist;
				int n;
				char path[500];
				sprintf(path, "%s/", getenv( "PWD" ));

			    n = scandir(path, &namelist, 0, alphasort);
				if (n < 0){
					perror("scandir");
					printf("%s\n", path);
				}
				else {
					while (n > 2) {
						n--;
						if(strncmp (namelist[n]->d_name,".",1) != 0){
							printf("%s   ", namelist[n]->d_name);
						}
					}
					printf("\n");
					free(namelist);
				}
			}				
			
			else{
				printf("Unrecognised command: \"%s\"\n",cmdArray[0]);
			}
		}
  }
  
  return 0;

}
