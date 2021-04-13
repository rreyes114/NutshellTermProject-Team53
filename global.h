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

evTable varTable;

aTable aliasTable;

char cwd[PATH_MAX];

int aliasIndex, varIndex;

char* subAliases(char* name);