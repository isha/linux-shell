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
#define COMMAND_DELIMETERS " \t"
#define PATH_DELIMETERS ":"

//Splits the input string into segments depending on DELIMITERS, passes all parts into the array cmdArray, and returns the number of segments in the array.
int parseString(char* inputString, char*** cmdArray, char* delimeters) {

	char* buffer;
	int numberOfCmds;

	buffer = (char*) malloc(strlen(inputString) * sizeof(char));
	strcpy(buffer,inputString);
	(*cmdArray) = (char**) malloc(MAX_ARGUMENTS * sizeof(char**));

	numberOfCmds = 0;
	(*cmdArray)[numberOfCmds++] = strtok(buffer, delimeters);
  
	while ((((*cmdArray)[numberOfCmds] = strtok(NULL, delimeters)) != NULL) && (numberOfCmds < MAX_ARGUMENTS)) {
		++numberOfCmds;
  }

return numberOfCmds;
}

void prompt() {
	char* user = malloc( strlen(getenv("USER"))*sizeof(char) );
	char* system = malloc( MAX_STRING_LENGTH * sizeof(char) );
	char* home = malloc( strlen(getenv("HOME")) );
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
	extern int alphasort();
  

	while(1){

		prompt();
		gets(inputString);

		if (strcmp(inputString,"") != 0){

			numberOfCmds = parseString(inputString, &cmdArray, COMMAND_DELIMETERS);

		if (strcmp(cmdArray[0],"exit") == 0){
		return 0;
		}

/* CHANGE CURRENT WORKING DIRECTORY
usage: cd [directory]
*/
		else if (strcmp(cmdArray[0],"cd") == 0){
			char home[MAX_STRING_LENGTH] = "";
            char directory[MAX_STRING_LENGTH] = "";
            char cwd[MAX_STRING_LENGTH] = "";
            strcpy( home, getenv("HOME") );

			if (numberOfCmds > 1){
                if(strncmp(cmdArray[1], "~", 1) == 0) {
                	memmove( cmdArray[1], cmdArray[1]+1, strlen(cmdArray[1]) );
                	strcpy( directory, home );
                }
                strcat( directory, cmdArray[1] );

                if ( chdir( directory ) == -1 ) printf("cd: invalid directory\n");
				else {
					getcwd(cwd, sizeof(cwd));
					setenv("PWD", cwd, 1);
				}
	  		} else {
                if ( chdir( home ) == -1 ) printf("cd: invalid directory\n");
				else {
					getcwd(cwd, sizeof(cwd));
					setenv("PWD", cwd, 1);
				}
            }
	}
/* RUN PROGRAM IN CURRENT WORKING DIRECTORY
usage: run [name] [arguments]
*/
	else if (strcmp(cmdArray[0],"run") == 0){
		if (numberOfCmds > 1){
			int pid = fork();
			if(pid == 0){
				int i;
				char* args[numberOfCmds-1];
				for( i = 0; i < numberOfCmds; i++ ) args[i] = cmdArray[i+1];
					char* pwd = malloc( strlen(getenv("PWD")) );
					execv( strcat(strcat(strcpy( pwd, getenv( "PWD" ) ),"/"), cmdArray[1] ), args );
			} 
			else if( pid == -1 ) {
				printf( "%s: forking failed\n", cmdArray[0] );
			}
                        else if (strcmp(cmdArray[numberOfCmds-1], "&" )!=0) {
                                // wait if last character not equal to &
                                wait(NULL);
                        } 
		} 
		else {
			printf("Not enough arguments.\n");
		}
	}

/* SET ENVIRONMENT VARIABLE
usage: setenv [name] [value]
*/
	else if (strcmp(cmdArray[0],"setenv") == 0){
		if( numberOfCmds > 2 ){
			if( setenv( cmdArray[1], cmdArray[2], 1 ) != 0 ) printf("Failed to set environment variable.\n");
		} 
		else {
			printf("Not enough arguments.\n");
		}
	}

/* GET ENVIRONMENT VARIABLE
usage: getenv [name]
*/
	else if (strcmp(cmdArray[0],"getenv") == 0){
		if( numberOfCmds > 1 ){
			if( getenv( cmdArray[1] ) != NULL ) printf( "%s : %s\n", cmdArray[1], getenv( cmdArray[1] ) );
			else printf( "Environment variable %s does not exist.\n", cmdArray[1] );
		}
		else {
			printf("Not enough arguments.\n");
		}
	}

/* LIST ALL FILES IN DIRECTORY
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
					printf("%s ", namelist[n]->d_name);
				}
			}
			printf("\n");
			free(namelist);
		}
	}

	/* PRINT HELP
	usage: help
	*/
                        else if (strcmp(cmdArray[0],"help") == 0){

				printf("\nhelp");
				printf("\n\ndir\t\t:\tList all files in directory");
				printf("\nUsage\t\t:\tdir\n");
                                printf("\n\ncd\t\t:\tChange present working directory");
                                printf("\nUsage\t\t:\tcd [path to new directory]\n");
                                printf("\n\ngetenv\t\t:\tShows the value of the specified Environment Variable");
                                printf("\nUsage\t\t:\tgetenv [name]\n");
                                printf("\n\nsetenv\t\t:\tSets the value of an Environment Variable");
                                printf("\nUsage\t\t:\tsetenv [name] [value]\n");
                                printf("\n\nunsetenv\t:\tDelete an Environment Variable");
                                printf("\nUsage\t\t:\tunsetenv [name]\n");
                                printf("\n\nrun\t\t:\tRun an executable");
                                printf("\nUsage\t\t:\trun [executable] [arguments]\n");



                        }

	

/* EXECUTE COMMAND
usage: [command] { arguments }
*/
	else {
		int isProgram = 0;
		int numPaths = 0;
		int j = 0;
		char* pathVar = malloc( sizeof(char)*strlen(getenv("PATH")) );
		char** paths;

		strcpy( pathVar, getenv("PATH") );

		numPaths = parseString( pathVar, &paths, PATH_DELIMETERS );
		for( j = 0; j < numPaths; j++ ) {
			char path[strlen(paths[j])];
			char *filename = strcat( strcat( strcpy( path, paths[j] ), "/" ), cmdArray[0] );
			if( access( filename, F_OK ) == 0 ) {
				int pid = fork();
					if(pid == 0){
						execv( filename, cmdArray );
					}
					else if( pid == -1 ) {
						printf( "%s: forking failed\n", cmdArray[0] );
					}
					else {
						wait(NULL);
					}
			isProgram = 1;
			break;
			}
		}
		if( !isProgram ) printf( "%s: command not found\n", cmdArray[0] );

	}
}
  }
  
  return 0;

}
