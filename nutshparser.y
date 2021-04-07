%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int yylex(void);
int yyerror(char *s);
%}

%union {char *string;}

%start cmd_line
%token <string> COMMAND WORD STRING NUMBER FLOAT RELATION FLAG FILEPATH

%%
cmd_line:
	COMMAND							{return 1;} //these will run functions later on
	| COMMAND STRING WORD			{return 1;}
	| COMMAND STRING				{return 1;}
	| COMMAND FILEPATH				{return 1;}
	| cmd_line RELATION cmd_line	{return 1;}
	| /* etc */;
%%

//functions for doing things
int yyerror(char *s) {
  printf("%s\n",s);
  return 0;
  }