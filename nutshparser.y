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
	| CD STRING				{return 1;}
	| ALIAS STRING STRING	{return 1;}
	| ALIAS					{return 1;}
	| UNALIAS STRING		{return 1;}
	| SETENV STRING STRING	{return 1;}
	| UNSETENV STRING		{return 1;}
	| PRINTENV				{return 1;}
	| /* etc */;
%%

//functions for doing things
int yyerror(char *s) {
	printf("%s\n",s);
	return 0;
}

//built-in commands
int runCD(char* arg) {
	
}

int runSetAlias(char *name, char *word) {
	
}

int runPrintAlias(void) {
	
}

int runUnalias(char *name) {
	
}

int runSetEnv(char *variable, char *word) {
	
}

int runUnsetEnv(char *name) {
	
}

int runPrintEnv(void) {
	
}
