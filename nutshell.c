// This is ONLY a demo micro-shell whose purpose is to illustrate the need for and how to handle nested alias substitutions and how to use Flex start conditions.
// This is to help students learn these specific capabilities, the code is by far not a complete nutshell by any means.
// Only "alias name word", "cd word", and "bye" run.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

char *getcwd(char *buf, size_t size);
int yyparse();
void clearCmdTable();
int executeCommand(char *command, int commandIndex);
void runPipedCommands();
void printTable();
bool wildCardMatch(char* filename, char* pattern, int n, int m);

int main()
{
    aliasIndex = 0;
    varIndex = 0;
	cmdIndex = 0;
    in = false;
    out = false;

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
			//check for wildcards
			int numArgs = cmdTable.argcnt[0];
			int wildIndex = -1;
			for(int i = 0; i < numArgs; i++) {
				if( (strchr(cmdTable.args[0][i], '*') != NULL) || (strchr(cmdTable.args[0][i], '?') != NULL) ) {
					wildIndex = i; break;
				}	
			}
			//printf("wildIndex: %i\n", wildIndex);
			//if a wild argument found
			if (wildIndex != -1) {
				char pattern[100];
				strcpy(pattern, cmdTable.args[0][wildIndex]);
				//search directory
				DIR *dir;
				struct dirent *ent;
				if( (dir = opendir(varTable.word[0])) != NULL) {
					while ( (ent = readdir(dir)) != NULL) {
						if( wildCardMatch(ent->d_name, pattern, strlen(ent->d_name), strlen(pattern)) ) {
							strcpy(cmdTable.args[0][wildIndex], ent->d_name);
							int pid = fork();
							if (pid == 0){
								executeCommand(cmdTable.name[cmdIndex-1], cmdIndex-1); 
							}
							wait(2);
						}
					}
				}
			}
			else {
				//execute commands
				int pid = fork();
				if (pid == 0){
					executeCommand(cmdTable.name[cmdIndex-1], cmdIndex-1); 
				}
				wait(2);
			}
        }       
		clearCmdTable();
    }

   return 0;
}

bool wildCardMatch(char* filename, char* pattern, int n, int m) { //n is strlen(filename), m is strlen(pattern)
	if (m == 0)
		return (n == 0);
	bool lookup[n+1][m+1];
	memset(lookup, false, sizeof(lookup));
	lookup[0][0] = true;
	
	for(int j = 1; j <= m; j++)
		if(pattern[j-1] == '*')
			lookup[0][j] = lookup[0][j-1];
	
	for(int i = 1; i <= n; i++) {
		for(int j = 1; j <= m; j++) {
			if(pattern[j-1] == '*')
				lookup[i][j] = lookup[i][j-1] || lookup[i-1][j];
			else if(pattern[j-1] == '?' || filename[i-1] == pattern[j-1])
				lookup[i][j] = lookup[i-1][j-1];
			else
				lookup[i][j] = false;
		}
	}
	return lookup[n][m];
}

void runPipedCommands() {
	int numPipes = cmdIndex-1;
	//printf("Number of pipes: %i\n", numPipes);
	printf("\n"); //without this, the pipes won't work -> LEAVE THIS IN
	pid_t pid;
	int pipefds[2*numPipes]; //Initialize pipe
	for(int i = 0; i < 2*numPipes; i++) {
		if(pipe(pipefds + i*2) < 0) { printf("Error: pipe failed to initialize.\n"); return; }
	}
	
	for(int i = 0; i < cmdIndex; i++) {
		//printf("I will fork\n");
		pid = fork(); //fork process
		//printf("I just forked\n");
		//printf(" ");
		if (pid < 0) { printf("Error: fork failed.\n"); return; }
		
		if(pid == 0) {
			//child
			if(i != 0) { //if not first command, read from prev pipe
				if(dup2(pipefds[(i-1)*2], 0) < 0) { printf("dup2 read error.\n"); return;}
			}
			if(i != cmdIndex-1) { //if not last command, write to next pipe
				if(dup2(pipefds[i*2 + 1], 1) < 0) { printf("dup2 write error.\n"); return;}
			}
			
			for(int j = 0; j < 2*numPipes; j++) {
				close(pipefds[i]);
			}
			//execute command
            //char* argList[100];
			//fix argList
			//int argCount = cmdTable.argcnt[i];
            //argList[0] = &cmdTable.name[i];
            //for (int k = 1; k < argCount+1; k++){
            //    argList[k] = &cmdTable.args[i][k-1];
            //}
            //argList[argCount+1] = NULL;
			//int pid2 = fork();
			//if (pid2 == 0){
			executeCommand(cmdTable.name[i], i);
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

int executeCommand(char *command, int commandIndex){

    //printf("in execute command \n");

    //create copy of args listed in command table
    char* argList[100];
    //char argList[128][100]
    int argCount = cmdTable.argcnt[commandIndex];
    argList[0] = &cmdTable.name[commandIndex];
    for (int i = 1; i < argCount+1; i++){
        argList[i] = &cmdTable.args[commandIndex][i-1];
        //strcpy(argList[i], cmdTable.args[cmdIndex-1][i]);
    }
    argList[argCount+1] = NULL;

    //get current PATH value from env table
    char pathvar[100];
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

        //initialize path on stack, with max length of 1000 - this way no memory is dynamically allocated
        char filePath[1000];
        strcpy(filePath, currentPath);
        strcat(filePath, "/");
        strcat(filePath, command);


        if (access(filePath, F_OK) == 0){
            //file does exist, execute with execv()
            //int pid = fork();
            //if (pid == 0){
                if (in)
                {
                    int fd0 = open(cmdTable.infile[commandIndex], O_RDONLY);
                    dup2(fd0, STDIN_FILENO);
                    close(fd0);
                }

                if (out)
                {
                    int fd1 = creat(cmdTable.outfile[commandIndex] , 0644) ;
                    dup2(fd1, STDOUT_FILENO);
                    close(fd1);
                }
                // child process, call execute here
                //search for and execute command, if exists somewhere in PATH variable
                execv(filePath, argList);
                printf("execv failed");
            //}
            //wait(2);
            return 1;
        }
        //print current path just to debug PATH parsing
		//printf("'%s'\n", currentPath);
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
	in = false;
	out = false;
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