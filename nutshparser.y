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
	BYE	END						{exit(1); return 1;}
	| CD STRING	END				{runCD($2); return 1;}
	| ALIAS STRING STRING END	{runSetAlias($2, $3); return 1;}
	| ALIAS	END					{runPrintAlias(); return 1;}
	| UNALIAS STRING END		{runUnalias($2); return 1;}
	| SETENV STRING STRING END	{runSetEnv($2, $3); return 1;}
	| UNSETENV STRING END		{runUnsetEnv($2); return 1;}
	| PRINTENV END				{runPrintEnv(); return 1;}
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
		printf("%s\t%s\n", aliasTable.name[i], aliasTable.word[i]);
	}
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
