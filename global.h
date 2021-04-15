#include "stdbool.h"
#include <limits.h>

typedef struct evTable { 
   char var[128][100];
   char word[128][100];
} evTable;

typedef struct aTable {
	char name[128][100];
	char word[128][100];
} aTable;

typedef struct cTable {
	char name[128][100];
	int argcnt[128];
	char args[128][128][100];
	char infile[128][100];
	char outfile[128][100];
} cTable;

/*
typedef struct bcommand {
	char name[16];
	int argcnt;
	char args[128][100];
	char infile[16];
	char outfile[16];
} bcommand;

typedef struct cTable {
	bcommand bcommands[128];
} cTable;
*/

evTable varTable;

aTable aliasTable;

cTable cmdTable;

int aliasIndex, varIndex, cmdIndex;

bool in, out;

char cwd[PATH_MAX];

char* subAliases(char* name);