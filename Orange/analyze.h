#pragma once
#include "globals.h"
#include "symtab.h"

typedef struct {
	Symtab* symtab;
	int location;
	int error;
} Analyzer;

void analyzer_init(Analyzer* analyzer, Symtab* symtab);
void buildSymtab(Analyzer*, TreeNode*);
void typeCheck(Analyzer*, TreeNode*);