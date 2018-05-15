#pragma once
#include "globals.h"
#include "symtab.h"
#define pc 7
#define mp 6
#define gp 5
#define ac 0
#define ac1 1

typedef struct {
	int emitLoc;
	int highEmitLoc;
	FILE* out;
	int traceCode;
	int tmpOffset;
	Symtab* symtab;
} Coder;

void coder_init(Coder* coder,FILE* out, Symtab* symtab,int traceCode);

void emitRO(Coder* coder,char* op, int r, int s, int t, char* c);

void emitRM(Coder* coder, char* op, int r, int d, int s, char* c);

int emitSkip(Coder* coder, int howMany);

void emitBackup(Coder* coder, int loc);

void emitRestore(Coder* coder);

void emitComment(Coder* coder, char* c);

void emitRM_Abs(Coder* coder, char* op, int r, int a, char* c);