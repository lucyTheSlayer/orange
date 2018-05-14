#include "util.h"

void printToken(TokenType token, const char* tokenString) {
	switch (token) {
	case IF:
	case ELSE:
	case DO:
	case FOR:
	case WHILE:
	case BREAK:
	case CONTINUE:
	case READ:
	case WRITE:
		printf("reserved word: %s\n", tokenString);
		break;
	case ASSIGN:
		printf(":=\n");
		break;
	case EQ:
		printf("=\n");
		break;
	case LT:
		printf("<\n");
		break;
	case GT:
		printf(">\n");
		break;
	case PLUS:
		printf("+\n");
		break;
	case MINUS:
		printf("-\n");
		break;
	case TIMES:
		printf("*\n");
		break;
	case OVER:
		printf("/\n");
		break;
	case LPARAM:
		printf("(\n");
		break;
	case RPARAM:
		printf(")\n");
		break;
	case SEMI:
		printf(";\n");
		break;
	case LBRACE:
		printf("{\n");
		break;
	case RBRACE:
		printf("}\n");
		break;
	case ENDFILE:
		printf("EOF\n");
		break;
	case NUM:
		printf("NUM, val= %s\n", tokenString);
		break;
	case ID:
		printf("ID, name= %s\n", tokenString);
		break;
	case ERROR:
		printf("ERROR : %s\n", tokenString);
		break;
	default:
		break;
		
	}
}

TreeNode* newStmtNode(StmtKind kind,int lineno) {
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL) {
		printf("out of memory at line %d\n", lineno);
	}
	else {
		for (i = 0; i < MAXCHILDREN; i++) {
			t->child[i] = NULL;
		}
		t->sibling = NULL;
		t->nodekind = StmtK;
		t->kind.stmt = kind;
		t->lineno = lineno;
	}
	return t;
}
TreeNode* newExpNode(ExpKind kind, int lineno) {
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL) {
		printf("out of memory at line %d\n",lineno);
	}
	else {
		for (i = 0; i < MAXCHILDREN; i++) {
			t->child[i] = NULL;
		}
		t->sibling = NULL;
		t->nodekind = ExpK;
		t->kind.exp = kind;
		t->lineno = lineno;
		t->type = Void;
	}
	return t;
}
char* copyString(char* s) {
	int n;
	char* t;
	if (s == NULL) {
		return NULL;
	}
	n = strlen(s) + 1;
	t = malloc(n);
	if (t == NULL) {
		return NULL;
	}
	else {
		strcpy(t, s);
	}
	return t;
}


static void printSpaces(int indentno) {
	for (int i = 0; i < indentno; i++) {
		printf(" ");
	}
}

static void _printTree(TreeNode* tree, int indentno) {
	int i;
	indentno += 2;
	while (tree != NULL) {
		printSpaces(indentno);
		if (tree->nodekind == StmtK) {
			switch (tree->kind.stmt) {
			case IfK:
				printf("If\n");
				break;
			case ForK:
				printf("For\n");
				break;
			case WhileK:
				printf("While\n");
				break;
			case DoWhileK:
				printf("DoWhile\n");
				break;
			case AssignK:
				printf("Assign to : %s\n", tree->attr.name);
				break;
			case ReadK:
				printf("Read: %s\n", tree->attr.name);
				break;
			case WriteK:
				printf("Write\n");
				break;
			default:
				printf("unknown expnode kind\n");
				break;
			}
		}
		else if (tree->nodekind == ExpK) {
			switch (tree->kind.exp) {
			case OpK:
				printf("Op: ");
				printToken(tree->attr.op, "\0");
				break;
			case ConstK:
				printf("Const: %d\n", tree->attr.val);
				break;
			case IdK:
				printf("Id: %s\n", tree->attr.name);
				break;
			default:
				printf("unknown expnode kind\n");
				break;
			}
		}
		else {
			printf("unknown node kind\n");
		}
		for (i = 0; i < MAXCHILDREN; i++) {
			_printTree(tree->child[i], indentno);
		}
		tree = tree->sibling;
	}
	indentno -= 2;
}

void printTree(TreeNode* tree) {
	_printTree(tree, 0);
}