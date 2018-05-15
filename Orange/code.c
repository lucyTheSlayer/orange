#include "code.h"

void coder_init(Coder* coder, FILE* out, Symtab* symtab, int traceCode) {
	coder->emitLoc = 0;
	coder->highEmitLoc = 0;
	coder->out = out;
	coder->traceCode = traceCode;
	coder->tmpOffset = 0;
	coder->symtab = symtab;
}

void emitRO(Coder* coder, char* op, int r, int s, int t, char* c) {
	fprintf(coder->out, "%3d:  %5s  %d,%d,%d ", coder->emitLoc++, op, r, s, t);
	if (coder->traceCode) {
		fprintf(coder->out, "\t%s", c);
	}
	fprintf(coder->out, "\n");
	if (coder->highEmitLoc < coder->emitLoc) {
		coder->highEmitLoc = coder->emitLoc;
	}
}

void emitRM(Coder* coder, char* op, int r, int d, int s, char* c) {
	fprintf(coder->out, "%3d:  %5s  %d,%d(%d) ", coder->emitLoc++, op, r, d, s);
	if (coder->traceCode) {
		fprintf(coder->out, "\t%s", c);
	}
	fprintf(coder->out, "\n");
	if (coder->highEmitLoc < coder->emitLoc) {
		coder->highEmitLoc = coder->emitLoc;
	}
}

int emitSkip(Coder* coder, int howMany) {
	int i = coder->emitLoc;
	coder->emitLoc += howMany;
	if (coder->highEmitLoc < coder->emitLoc) {
		coder->highEmitLoc = coder->emitLoc;
	}
	return i;
}

void emitBackup(Coder* coder, int loc) {
	if (loc > coder->highEmitLoc) {
		emitComment(coder,"BUG in emitBackup");
	}
	coder->emitLoc = loc;
}

void emitRestore(Coder* coder) {
	coder->emitLoc = coder->highEmitLoc;
}

void emitComment(Coder* coder, char* c) {
	if (coder->traceCode) {
		fprintf(coder->out, "* %s\n", c);
	}
}

void emitRM_Abs(Coder* coder, char* op, int r, int a, char* c) {
	fprintf(coder->out, "%3d:  %5s  %d,%d(%d) ", coder->emitLoc, op, r, a - (coder->emitLoc + 1), pc);
	coder->emitLoc++;
	if (coder->traceCode) {
		fprintf(coder->out, "\t%s", c);
	}
	fprintf(coder->out, "\n");
	if (coder->highEmitLoc < coder->emitLoc) {
		coder->highEmitLoc = coder->emitLoc;
	}
}