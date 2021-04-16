%{

#include "nutshparser.tab.h"
#include <string.h>
#include "global.h"

char* subAliases(char* name){
    for (int i = 0; i < aliasIndex; i++) {
        if(strcmp(aliasTable.name[i], name) == 0) {
            return aliasTable.word[i];
        }
    }
    return name;
}
bool ifAlias(char* name){
    for (int i = 0; i < aliasIndex; i++) {
        if(strcmp(aliasTable.name[i], name) == 0) {
            return true;
        }
    }
    return false;
}

char* subEnvs(char* name){
    for (int i = 0; i < envIndex; i++) {
        if(strcmp(envTable.variable[i], name) == 0) {
            return envTable.word[i];
        }
    }
    return name;
}
bool ifEnv(char* name){
    for (int i = 0; i < varIndex; i++) {
        if(strcmp(envTable.variable[i], name) == 0) {
            return true;
        }
    }
    return false;
}
%}
%array
%option noyywrap

%x string_condition

CHAR            [.A-Za-z0-9!\/_-][.A-Za-z0-9!\/_-]*
ANYCHAR            [)(*&%$#@!`;,\.a-zA-Z0-9'/*_=~ -][)(*&%$#@!`;,\.a-zA-Z0-9'/*_=~ -]*

%x env_condition

CHAR            [.A-Za-z0-9!\/_-][.A-Za-z0-9!\/_-]*
ANYCHAR            [)(*&%$#@!`;,\.a-zA-Z0-9'/*_=~ -][)(*&%$#@!`;,\.a-zA-Z0-9'/*_=~ -]*

%%




<string_condition>{ANYCHAR}+     { yylval.string = strdup(yytext); return STRING;}
<string_condition>[\"]          {BEGIN(INITIAL);}

<env_condition>[\}]              {BEGIN(INITIAL);}
<env_condition>{ANYCHAR}+          { yylval.string = strdup(yytext); return ENV;}

[ ]		            { }
bye                 { return BYE; }
cd		            { return CD;}
alias		        { return ALIAS; }
unalias             { return UNALIAS; }
setenv              { return SETENV; }
unsetenv            { return UNSETENV; }
printenv            { return PRINTENV; }
"|"                 { return PIPE; }
">"                 { return GREATER; }
"<"                 { return LESS; }
"${"                 { BEGIN(env_condition);}
{CHAR}+"."{CHAR}+	{ yylval.string = strdup(yytext); return FILENAME; }
"\n"		        { return END; }
[\"]                { BEGIN(string_condition); }

{CHAR}+              {if(ifAlias(yytext)) {
                        printf("yytext: %s\n", yytext);
                        //source: https://www.cs.princeton.edu/~appel/modern/c/software/flex/flex.html
                           char *yycopy = strdup( subAliases(yytext) );
                           for ( int i = strlen(subAliases(yytext)) - 1; i >= 0; --i )
                               unput( yycopy[i] );
                           free( yycopy );
                    } elseif (ifEnv(yytext){
                        printf("yytext: %s\n", yytext);
                        //source: https://www.cs.princeton.edu/~appel/modern/c/software/flex/flex.html
                           char *yycopy = strdup( subEnvs(yytext) );
                           for ( int i = strlen(subEnvs(yytext)) - 1; i >= 0; --i )
                               unput( yycopy[i] );
                           free( yycopy );

                    })
                    else {
                        printf("yytext: %s\n", yytext);
                        yylval.string = strdup(yytext);
                        return STRING;
                    };
                     }
%%