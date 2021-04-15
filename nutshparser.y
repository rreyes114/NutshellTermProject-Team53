%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "global.h"

int yylex(void);
int yyerror(char *s);

//built-in commands prototypes
int runCD(char* arg);
int runSetAlias(char *name, char *word);
int runPrintAlias(void);
int runUnalias(char *name);
int runSetEnv(char *variable, char *word);
int runUnsetEnv(char *name);
int runPrintEnv(void);

//command table functions
int storeCommand(char *name);
int storeArgument(char *arg);
int storeInputFile(char *filename);
int storeOutputFile(char *filename);
%}

%union {char *string;}

%start commandList
%token <string> BYE CD STRING ALIAS END PIPE UNALIAS SETENV UNSETENV PRINTENV GREATER LESS FILENAME

%% 
commandList:
	command PIPE commandList	{}
	| command END				{return 1;}
	;

command:
	BYE							{exit(1);}
	| CD STRING					{runCD($2);}
	| ALIAS STRING STRING 		{runSetAlias($2, $3);}
	| ALIAS						{runPrintAlias();}
	| UNALIAS STRING 			{runUnalias($2);}
	| SETENV STRING STRING 		{runSetEnv($2, $3);}
	| UNSETENV STRING 			{runUnsetEnv($2);}
	| PRINTENV 					{runPrintEnv();}
	| STRING argumentList LESS FILENAME GREATER FILENAME	{storeInputFile($4); storeOutputFile($6); storeCommand($1);}
	| STRING argumentList GREATER FILENAME 	{storeOutputFile($4); storeCommand($1);}
	| STRING argumentList LESS FILENAME 	{storeInputFile($4); storeCommand($1);}
	| STRING argumentList 		{storeCommand($1);}
	;
	
argumentList:
	argument argumentList	{}
	| %empty				{}
	;
	
argument:
	STRING 		{storeArgument($1);}
	;
	
%%

//functions for doing things
int yyerror(char *s) {
	printf("%s\n",s);
	return 0;
}

//command table functions
int storeCommand(char *name) {
	strcpy(cmdTable.name[cmdIndex], name);
	cmdIndex++;
	return 1;
}

int storeArgument(char *arg) {
	int argIndex = cmdTable.argcnt[cmdIndex];
	strcpy(cmdTable.args[cmdIndex][argIndex], arg);
	cmdTable.argcnt[cmdIndex]++;
	return 1;
}

int storeInputFile(char *filename) {
	strcpy(cmdTable.infile[cmdIndex], filename);
	in = true;
	return 1;
}

int storeOutputFile(char *filename) {
	strcpy(cmdTable.outfile[cmdIndex], filename);
	out = true;
	return 1;
}

//built-in commands
int runCD(char* arg) {
	if (arg[0] != '/') { // arg is relative path
		strcat(varTable.word[0], "/");
		strcat(varTable.word[0], arg);

		if(chdir(varTable.word[0]) == 0) {
			return 1;
		}
		else {
			getcwd(cwd, sizeof(cwd));
			strcpy(varTable.word[0], cwd);
			printf("Directory not found\n");
			return 1;
		}
	}
	else { // arg is absolute path
		if(chdir(arg) == 0){
			strcpy(varTable.word[0], arg);
			return 1;
		}
		else {
			printf("Directory not found\n");
            return 1;
		}
	}	
}

int runSetAlias(char *name, char *word) {
	for (int i = 0; i < aliasIndex; i++) {
		if(strcmp(name, word) == 0){
			printf("Error, expansion of \"%s\" would create a loop.\n", name);
			return 1;
		}
		else if((strcmp(aliasTable.name[i], name) == 0) && (strcmp(aliasTable.word[i], word) == 0)){
			printf("Error, expansion of \"%s\" would create a loop.\n", name);
			return 1;
		}
		else if(strcmp(aliasTable.name[i], name) == 0) {
			strcpy(aliasTable.word[i], word);
			return 1;
		}
	}
	strcpy(aliasTable.name[aliasIndex], name);
	strcpy(aliasTable.word[aliasIndex], word);
	aliasIndex++;

	return 1;
}

int runPrintAlias(void) {
	for (int i = 0; i < aliasIndex; i++) {
		printf("%s\t%s\n", aliasTable.name[i], aliasTable.word[i]);
	}
	/*
	//printing command Table also for debugging purposes - COMMENT THIS OUT
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
	*/
}

int runUnalias(char *name) {
	for (int i = 0; i < aliasIndex; i++) {
		//swap alias at found index with last valid alias, then decrement counter
		if(strcmp(aliasTable.name[i], name) == 0){
			strcpy(aliasTable.name[i], aliasTable.name[aliasIndex-1]);
			strcpy(aliasTable.word[i], aliasTable.word[aliasIndex-1]);
			aliasIndex--;
			break;
		}
	}
	return 1;
}

int runSetEnv(char *variable, char *word) {
	for (int i = 0; i < varIndex; i++) {
		if(strcmp(variable, word) == 0){
			printf("Error, expansion of \"%s\" would create a loop.\n", variable);
			return 1;
		}
		else if((strcmp(varTable.var[i], variable) == 0) && (strcmp(varTable.word[i], word) == 0)){
			printf("Error, expansion of \"%s\" would create a loop.\n", variable);
			return 1;
		}
		else if(strcmp(varTable.var[i], variable) == 0) {
			strcpy(varTable.var[i], variable);
			return 1;
		}
	}
	strcpy(varTable.var[varIndex], variable);
	strcpy(varTable.word[varIndex], word);
	varIndex++;
	return 1;
}

int runUnsetEnv(char *variable) {
	for (int i = 0; i < varIndex; i++) {
		//swap var at found index with last valid var, then decrement counter
		if(strcmp(varTable.var[i], variable) == 0){
			strcpy(varTable.var[i], varTable.var[varIndex-1]);
			strcpy(varTable.word[i], varTable.word[varIndex-1]);
			varIndex--;
			break;
		}
	}
	return 1;
}

int runPrintEnv(void) {
	for (int i = 0; i < varIndex; i++) {
		printf("%s\t%s\n", varTable.var[i], varTable.word[i]);
	}
}
