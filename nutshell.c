// This is ONLY a demo micro-shell whose purpose is to illustrate the need for and how to handle nested alias substitutions and how to use Flex start conditions.
// This is to help students learn these specific capabilities, the code is by far not a complete nutshell by any means.
// Only "alias name word", "cd word", and "bye" run.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include <unistd.h>

char *getcwd(char *buf, size_t size);
int yyparse();
void clearCmdTable();
int executeCommand(char *command, char **args);
void printTable();


int main()
{
    aliasIndex = 0;
    varIndex = 0;
	cmdIndex = 0;

    getcwd(cwd, sizeof(cwd));

    strcpy(varTable.var[varIndex], "PWD");
    strcpy(varTable.word[varIndex], cwd);
    varIndex++;
    strcpy(varTable.var[varIndex], "HOME");
    strcpy(varTable.word[varIndex], cwd);
    varIndex++;
    strcpy(varTable.var[varIndex], "PROMPT");
    strcpy(varTable.word[varIndex], "nutshell");
    varIndex++;
    strcpy(varTable.var[varIndex], "PATH");
    strcpy(varTable.word[varIndex], ".:/bin");
    varIndex++;

    system("clear");
    while(1)
    {
        printf("[%s]>> ", varTable.word[2]);
        yyparse();
		
		//pipes and io
		
		//execute commands
        while (cmdIndex > 0){
            printTable();
            //create copy of args listed in command table
            char* argList[100];
            //char argList[128][100]
            int argCount = cmdTable.argcnt[cmdIndex-1];
            argList[0] = &cmdTable.name[cmdIndex-1];
            for (int i = 1; i < argCount+1; i++){
                argList[i] = &cmdTable.args[cmdIndex-1][i-1];
                //strcpy(argList[i], cmdTable.args[cmdIndex-1][i]);
            }
            argList[argCount+1] = NULL;

            int pid = fork();
            if (pid == 0){
                // child process, call execute here
                //search for and execute command, if exists somewhere in PATH variable
                executeCommand(cmdTable.name[cmdIndex-1], argList);
            }
            //wait for 2 seconds for child process to finish
            wait(2);
            cmdIndex--;
        }
		
		clearCmdTable();
    }

   return 0;
}

int executeCommand(char *command, char** args){

    //get current PATH value from env table
    char* pathvar;
    for (int i = 0; i < varIndex; i++) {
		if (strcmp(varTable.var[i], "PATH") == 0){
            strcpy(pathvar, varTable.word[i]);
            break;
        }
	}

    //split path
    char *currentPath = strtok(pathvar, ":");
	while(currentPath != NULL)
	{
        printf("'%s'\n", currentPath);

        //initialize path on stack, with max length of 1000 - this way no memory is dynamically allocated
        char filePath[1000];
        strcpy(filePath, currentPath);
        strcat(filePath, "/");
        strcat(filePath, command);

        printf("%s", filePath);

        if (access(filePath, F_OK) == 0){
            //file does exist, execute with execv()
            execv(filePath, args);
            printf("Successfully called %s in PATH directory %s", command, currentPath);
            return 1;
        }
        //print current path just to debug PATH parsing
		printf("'%s'\n", currentPath);
		currentPath = strtok(NULL, ":");
	}
    //nothing was found 
    printf("Could not find executable for command %s in any of the directories specified in PATH variable", command);
    return 1;
}


void clearCmdTable() {
	for(int i = 0; i < cmdIndex; i++) {
		memset(cmdTable.name[i], 0, sizeof(cmdTable.name[i])); //clear name
		int numArgs = cmdTable.argcnt[i];
		for (int j = 0; j < numArgs; j++) {
			memset(cmdTable.args[i][j], 0, sizeof(cmdTable.args[i][j])); //clear arguments
		}
		cmdTable.argcnt[i] = 0; //clear arg count
		memset(cmdTable.infile[i], 0, sizeof(cmdTable.infile[i])); //clear input file
		memset(cmdTable.outfile[i], 0, sizeof(cmdTable.outfile[i])); //clear output file
	}
	cmdIndex = 0;
}

void printTable(){
    printf("Number of commands: %i\n", cmdIndex);
	for (int i = 0; i < cmdIndex; i++) {
		printf("Command %i: %s\n", i, cmdTable.name[i]);
		int numArgs = cmdTable.argcnt[i];
		for (int j = 0; j < numArgs; j++) {
			printf("\t%s", cmdTable.args[i][j]);
		}
		printf("\n");
		printf("\tInput File: %s\n", cmdTable.infile[i]);
		printf("\tOutput File: %s\n", cmdTable.outfile[i]);
	}
}