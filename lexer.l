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
%}
%array
%option noyywrap

%x string_condition

CHAR            [.A-Za-z0-9!\/_-][.A-Za-z0-9!\/_-]*
WILDCHAR            [.A-Za-z0-9!\/_*?-][.A-Za-z0-9!\/_*?-]*
ANYCHAR            [)(*&%$#@!`;,\.a-zA-Z0-9'/*_=~ -][)(*&%$#@!`;,\.a-zA-Z0-9'/*_=~ -]*

%%

<string_condition>{ANYCHAR}+     { yylval.string = strdup(yytext); return STRING;}
<string_condition>[\"]          {BEGIN(INITIAL);}

[ ]		            { }
bye                 { return BYE; }
cd		            { return CD;}
alias		        { return ALIAS; }
unalias             { return UNALIAS; }
setenv              { return SETENV; }
unsetenv            { return UNSETENV; }
printenv            { return PRINTENV; }
2>					{ return REDERR; }
&1					{ return STDOUT; }
"|"                 { return PIPE; }
">"                 { return GREATER; }
"<"                 { return LESS; }
{WILDCHAR}+"."{CHAR}+	{ yylval.string = strdup(yytext); return FILENAME; }
"\n"		        { return END; }
[\"]                { BEGIN(string_condition); }

{CHAR}+              {if(ifAlias(yytext)) {
                        //source: https://www.cs.princeton.edu/~appel/modern/c/software/flex/flex.html
                           char *yycopy = strdup( subAliases(yytext) );
                           for ( int i = strlen(subAliases(yytext)) - 1; i >= 0; --i )
                               unput( yycopy[i] );
                           free( yycopy );
                    } else {
                        yylval.string = strdup(yytext);
                        return STRING;
                    };
                     }
%%