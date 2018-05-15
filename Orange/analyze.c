#include "analyze.h"


void analyzer_init(Analyzer* analyzer, Symtab* symtab) {
	analyzer->symtab = symtab;
	analyzer->location = 0;
	analyzer->error = FALSE;
}

static void traverse(Analyzer* analyzer,TreeNode* t, void(*preProc)(Analyzer*,TreeNode*), void(*postProc)(Analyzer*,TreeNode*)) {
	if (t != NULL) {
		preProc(analyzer,t);
		for (int i = 0; i < MAXCHILDREN; i++) {
			traverse(analyzer,t->child[i], preProc, postProc);
		}
		postProc(analyzer,t);
		traverse(analyzer,t->sibling, preProc, postProc);
	}
}

static void nullProc(Analyzer* analyzer, TreeNode* t) {
	//if (t == NULL) {
	//	return;
	//}
	//else {
	//	return;
	//}
	return;
}

static void insertNode(Analyzer* analyzer,TreeNode* t) {
	switch (t->nodekind) {
	case StmtK:
		switch (t->kind.stmt) {
		case AssignK:
		case ReadK:
			if (st_lookup(analyzer->symtab, t->attr.name) == -1) {
				st_insert(analyzer->symtab,t->attr.name, t->lineno, analyzer->location++);
			}
			else {
				st_insert(analyzer->symtab, t->attr.name, t->lineno, 0);
			}
			break;
		default:
			break;
		}
		break;
	case ExpK:
		switch (t->kind.exp) {
		case IdK:
			if (st_lookup(analyzer->symtab, t->attr.name) == -1) {
				st_insert(analyzer->symtab, t->attr.name, t->lineno, analyzer->location++);
			}
			else {
				st_insert(analyzer->symtab, t->attr.name, t->lineno, 0);
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void buildSymtab(Analyzer* analyzer, TreeNode* syntaxTree) {
	traverse(analyzer,syntaxTree, insertNode, nullProc);
	printf("\nSymbol table:\n\n");
	printSymTab(analyzer->symtab);
}


static void typeError(Analyzer* analyzer, TreeNode* t, char* message) {
	printf("Type error at line %d: %s\n", t->lineno, message);
	analyzer->error = TRUE;
}

static void checkNode(Analyzer* analyzer, TreeNode* t) {
	switch (t->nodekind) {
	case ExpK:
		switch (t->kind.exp) {
		case OpK:
			if (t->child[0]->type != Integer || t->child[1]->type != Integer) {
				typeError(analyzer, t, "Op applied to non-integer");
			}
			if (t->attr.op == EQ || t->attr.op == LT || t->attr.op == GT) {
				t->type = Boolean;
			}
			else {
				t->type = Integer;
			}
			break;
		case ConstK:
		case IdK:
			t->type = Integer;
			break;
		default:
			break;
		}
		break;
	case StmtK:
		switch (t->kind.stmt) {
		case IfK:
			if (t->child[0]->type == Integer) {
				typeError(analyzer,t->child[0], "if test is not Boolean");
			}
			break;
		case AssignK:
			if (t->child[0]->type != Integer) {
				typeError(analyzer, t->child[0], "assignment of non-integer value");
			}
			break;
		case WriteK:
			if (t->child[0]->type != Integer) {
				typeError(analyzer, t->child[0], "write of non-integer value");
			}
			break;
		case WhileK:
			if (t->child[0]->type == Integer) {
				typeError(analyzer, t->child[0], "while test is not Boolean");
			}
			break;
		case DoWhileK:
			if (t->child[1]->type == Integer) {
				typeError(analyzer, t->child[1], "do-while test is not Boolean");
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}


void typeCheck(Analyzer* analyzer, TreeNode* syntaxTree) {
	traverse(analyzer, syntaxTree, nullProc, checkNode);
}
