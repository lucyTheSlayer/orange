#pragma once
#include "globals.h"

#define SIZE 211
typedef struct LineListRec {
	int lineno;
	struct LineListRec* next;
}* LineList;

typedef struct BucketListRec {
	char* name;
	LineList lines;
	int memloc;
	struct BucketListRec* next;
}* BucketList;

typedef struct {
	BucketList hashTable[SIZE];
} Symtab;

void st_init(Symtab* symtab);
void st_insert(Symtab* symtab,char* name, int lineno, int loc);
int st_lookup(Symtab* symtab,char* name);
void printSymTab(Symtab* symtab);