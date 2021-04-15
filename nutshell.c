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
void runPipedCommands();

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
		if(cmdIndex > 1) {
			runPipedCommands();
		}
		else if(cmdIndex == 1){
		//execute commands
			//for(int j = 0; j < cmdIndex; j++) {
			//while(cmdIndex > 0) {
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
				//cmdIndex--;
			//}
		}
		
		clearCmdTable();
    }

   return 0;
}

void runPipedCommands() {
	int numPipes = cmdIndex-1;
	//printf("Number of pipes: %i\n", numPipes);
	printf("\n"); //without this, the pipes won't work -> LEAVE THIS IN
	pid_t pid;
	int pipefds[2*numPipes]; //Initialize pipe.
	for(int i = 0; i < 2*numPipes; i++) {
		if(pipe(pipefds + i*2) < 0) { printf("Error: pipe failed to initialize.\n"); return; }
	}
	
	for(int i = 0; i < cmdIndex; i++) {
		pid = fork(); //fork process
		//printf("I just forked\n");
		//printf(" ");
		if (pid < 0) { printf("Error: fork failed.\n"); return; }
		
		if(pid == 0) {
			//child
			if(i != 0) { //if not first command, read from prev pipe
				if(dup2(pipefds[(i-1)*2], 0) < 0) { printf("dup2 error.\n"); return;}
			}
			if(i != cmdIndex-1) { //if not last command, write to next pipe
				if(dup2(pipefds[i*2 + 1], 1) < 0) { printf("dup2 error.\n"); return;}
			}
			
			for(int j = 0; j < 2*numPipes; j++) {
				close(pipefds[i]);
			}
			//execute command
            char* argList[100];
			//fix argList
			int argCount = cmdTable.argcnt[i];
            argList[0] = &cmdTable.name[i];
            for (int k = 1; k < argCount+1; k++){
                argList[k] = &cmdTable.args[i][k-1];
            }
            argList[argCount+1] = NULL;
			//int pid2 = fork();
			//if (pid2 == 0){
				executeCommand(cmdTable.name[i], argList);
			//}
			//wait(2);
			//exit(1); //there is a child escaping
		}
	}
	wait(5);
	//close pipe in parent
	for(int i = 0; i < 2 * numPipes; i++) {
		close(pipefds[i]);
		//puts("closed pipe in parent");
	}
	//printf("closed pipe in parent\n");
	//printf(" ");
	
	while(waitpid(0,0,0) <= 0);
}

int executeCommand(char *command, char **args){

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