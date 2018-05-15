#include "cgen.h"


static void cGen(Coder* coder, TreeNode* tree);

static void genStmt(Coder* coder, TreeNode* tree) {
	TreeNode *p1, *p2, *p3;
	int savedLoc1, savedLoc2, currentLoc;
	int loc;
	switch (tree->kind.stmt) {
	case IfK:
		if (coder->traceCode) {
			emitComment(coder, "-> if");
		}
		p1 = tree->child[0];
		p2 = tree->child[1];
		p3 = tree->child[2];
		/* generate code for test expression */
		cGen(coder, p1);
		savedLoc1 = emitSkip(coder, 1);
		emitComment(coder, "if: jump to else belongs here");
		/* recurse on then part */
		cGen(coder, p2);
		savedLoc2 = emitSkip(coder, 1);
		emitComment(coder, "if: jump to end belongs here");
		currentLoc = emitSkip(coder, 0);
		emitBackup(coder, savedLoc1);
		emitRM_Abs(coder, "JEQ", ac, currentLoc, "if: jmp to else");
		emitRestore(coder);
		/* recurse on else part */
		cGen(coder,p3);
		currentLoc = emitSkip(coder, 0);
		emitBackup(coder, savedLoc2);
		emitRM_Abs(coder, "LDA", pc, currentLoc, "jmp to end");
		emitRestore(coder);
		if (coder->traceCode)  emitComment(coder, "<- if");
		break; /* if_k */
	case WhileK:
		if(coder->traceCode) emitComment(coder, "-> while");
		p1 = tree->child[0];
		p2 = tree->child[1];
		savedLoc1 = emitSkip(coder, 0);
		cGen(coder, p1);
		savedLoc2 = emitSkip(coder, 1);
		emitComment(coder, "while: jump to end belongs here");
		cGen(coder, p2);
		currentLoc = emitSkip(coder, 0);
		emitRM_Abs(coder, "LDA", pc, savedLoc1, "jmp back to loop");
		emitBackup(coder,savedLoc2);
		emitRM_Abs(coder,"JEQ", ac, currentLoc, "while: jmp to end");
		emitRestore(coder);
		if (coder->traceCode)  emitComment(coder, "<- while");
		break; /* while_k */
	case AssignK:
		if (coder->traceCode) emitComment(coder, "-> assign");
		/* generate code for rhs */
		cGen(coder, tree->child[0]);
		/* now store value */
		loc = st_lookup(coder->symtab, tree->attr.name);
		emitRM(coder, "ST", ac, loc, gp, "assign: store value");
		if (coder->traceCode)  emitComment(coder, "<- assign");
		break; /* assign_k */

	case ReadK:
		emitRO(coder, "IN", ac, 0, 0, "read integer value");
		loc = st_lookup(coder->symtab, tree->attr.name);
		emitRM(coder, "ST", ac, loc, gp, "read: store value");
		break;
	case WriteK:
		/* generate code for expression to write */
		cGen(coder, tree->child[0]);
		/* now output it */
		emitRO(coder, "OUT", ac, 0, 0, "write ac");
		break;
	default:
		break;
	}
}

/* Procedure genExp generates code at an expression node */
static void genExp(Coder* coder,TreeNode * tree)
{
	int loc;
	TreeNode * p1, *p2;
	switch (tree->kind.exp) {

	case ConstK:
		if (coder->traceCode) emitComment(coder,"-> Const");
		/* gen code to load integer constant using LDC */
		emitRM(coder,"LDC", ac, tree->attr.val, 0, "load const");
		if (coder->traceCode)  emitComment(coder,"<- Const");
		break; /* ConstK */

	case IdK:
		if (coder->traceCode) emitComment(coder,"-> Id");
		loc = st_lookup(coder->symtab,tree->attr.name);
		emitRM(coder,"LD", ac, loc, gp, "load id value");
		if (coder->traceCode)  emitComment(coder,"<- Id");
		break; /* IdK */

	case OpK:
		if (coder->traceCode) emitComment(coder,"-> Op");
		p1 = tree->child[0];
		p2 = tree->child[1];
		/* gen code for ac = left arg */
		cGen(coder,p1);
		/* gen code to push left operand */
		emitRM(coder,"ST", ac, coder->tmpOffset--, mp, "op: push left");
		/* gen code for ac = right operand */
		cGen(coder,p2);
		/* now load left operand */
		emitRM(coder,"LD", ac1, ++coder->tmpOffset, mp, "op: load left");
		switch (tree->attr.op) {
		case PLUS:
			emitRO(coder,"ADD", ac, ac1, ac, "op +");
			break;
		case MINUS:
			emitRO(coder,"SUB", ac, ac1, ac, "op -");
			break;
		case TIMES:
			emitRO(coder,"MUL", ac, ac1, ac, "op *");
			break;
		case OVER:
			emitRO(coder,"DIV", ac, ac1, ac, "op /");
			break;
		case LT:
			emitRO(coder,"SUB", ac, ac1, ac, "op <");
			emitRM(coder,"JLT", ac, 2, pc, "br if true");
			emitRM(coder,"LDC", ac, 0, ac, "false case");
			emitRM(coder,"LDA", pc, 1, pc, "unconditional jmp");
			emitRM(coder,"LDC", ac, 1, ac, "true case");
			break;
		case EQ:
			emitRO(coder, "SUB", ac, ac1, ac, "op ==");
			emitRM(coder, "JEQ", ac, 2, pc, "br if true");
			emitRM(coder, "LDC", ac, 0, ac, "false case");
			emitRM(coder, "LDA", pc, 1, pc, "unconditional jmp");
			emitRM(coder, "LDC", ac, 1, ac, "true case");
			break;
		default:
			emitComment(coder, "BUG: Unknown operator");
			break;
		} /* case op */
		if (coder->traceCode)  emitComment(coder, "<- Op");
		break; /* OpK */

	default:
		break;
	}
} /* genExp */

  /* Procedure cGen recursively generates code by
  * tree traversal
  */
static void cGen(Coder* coder,TreeNode * tree)
{
	if (tree != NULL)
	{
		switch (tree->nodekind) {
		case StmtK:
			genStmt(coder,tree);
			break;
		case ExpK:
			genExp(coder,tree);
			break;
		default:
			break;
		}
		cGen(coder,tree->sibling);
	}
}

/**********************************************/
/* the primary function of the code generator */
/**********************************************/
/* Procedure codeGen generates code to a code
* file by traversal of the syntax tree. The
* second parameter (codefile) is the file name
* of the code file, and is used to print the
* file name as a comment in the code file
*/
void codeGen(Coder* coder, TreeNode* syntaxTree, char* codefile) {
	char * s = malloc(strlen(codefile) + 7);
	strcpy(s, "File: ");
	strcat(s, codefile);
	emitComment(coder,"TINY Compilation to TM Code");
	emitComment(coder, s);
	/* generate standard prelude */
	emitComment(coder, "Standard prelude:");
	emitRM(coder, "LD", mp, 0, ac, "load maxaddress from location 0");
	emitRM(coder, "ST", ac, 0, ac, "clear location 0");
	emitComment(coder, "End of standard prelude.");
	/* generate code for TINY program */
	cGen(coder, syntaxTree);
	/* finish */
	emitComment(coder, "End of execution.");
	emitRO(coder, "HALT", 0, 0, 0, "");
}