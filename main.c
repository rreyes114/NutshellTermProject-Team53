#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include <unistd.h>

char *getcwd(char *buf, size_t size);
int yyparse();

int main()
{
    aliasIndex = 0;
    varIndex = 0;

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


        //TODO: set up pipes and I/O as necessary

        while (cmdIndex > 0){
            executeCommand(cmdTable.name[cmdIndex-1], cmdTable.args[cmdIndex-1]);
        }
    }

   return 0;
}


int executeCommand(char *command, char *args){
    //get current PATH value from env table
    char* pathvar;
    for (int i = 0; i < varIndex; i++) {
		if (evtable.var[i] == "PATH"){
            pathvar = evtable.word[i];
            break;
        }
	}

    //split path
    char *currentPath = strtok(pathvar, ":");
	while(currentPath != NULL)
	{
        //initialize path on stack, with max length of 1000 - this way no memory is dynamically allocated
        char filePath[1000];
        strcpy(filepath, currentPath);
        strcat(filepath, "/");
        strcat(filepath, command);

        if (access(filePath, F_OK) == 0){
            //file does exist, execute with execv()
            execv(filePath, args);
            printf("Successfully called %s in PATH directory %s", command, currentPath);
            return 1;
        }
        //print current path just to debug PATH parsing
		printf("'%s'\n", currentPath);
		ptr = strtok(NULL, delim);
	}
    //nothing was found 
    printf("Could not find executable for command %s in any of the directories specified in PATH variable");
    return 1;
}
