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
		
		clearCmdTable();
    }

   return 0;
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