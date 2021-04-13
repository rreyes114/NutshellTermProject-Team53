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
%}

%union {char *string;}

%start commandList
%token <string> BYE CD STRING ALIAS END PIPE UNALIAS SETENV UNSETENV PRINTENV

%%
commandList:
	command PIPE commandList 	{/*wtf do I do here*/}
	| command END				{/*same*/}
	| /* other cases */;

command:
	BYE						{exit(1); return 1;}
	| CD STRING				{runCD($2); return 1;}
	| ALIAS STRING STRING	{runSetAlias($2, $3); return 1;}
	| ALIAS					{runPrintAlias(); return 1;}
	| UNALIAS STRING		{runUnalias($2); return 1;}
	| SETENV STRING STRING	{runSetEnv($2, $3); return 1;}
	| UNSETENV STRING		{runUnsetEnv($2); return 1;}
	| PRINTENV				{runPrintEnv(); return 1;}
	| /* etc */;
%%

//functions for doing things
int yyerror(char *s) {
	printf("%s\n",s);
	return 0;
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
		printf("%s\n", aliasTable.name[i]);
	}
}

int runUnalias(char *name) {
	
}

int runSetEnv(char *variable, char *word) {
	
}

int runUnsetEnv(char *name) {
	
}

int runPrintEnv(void) {
	for (int i = 0; i < varIndex; i++) {
		printf("%s\n", varTable.var[i]);
	}
}
