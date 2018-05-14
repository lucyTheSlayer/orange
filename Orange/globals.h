#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define FALSE 0
#define TRUE 1

#define MAXRESERVED 9

typedef enum {
	ENDFILE,ERROR,
	IF,ELSE,DO,FOR,WHILE,BREAK,CONTINUE,READ,WRITE,
	ID,NUM,
	ASSIGN,EQ,LT,GT,PLUS,MINUS,TIMES,OVER,LPARAM,RPARAM,SEMI,LBRACE,RBRACE
} TokenType;

typedef enum {StmtK,ExpK} NodeKind;
typedef enum {IfK,ForK,WhileK,DoWhileK,AssignK,ReadK,WriteK} StmtKind;
typedef enum {OpK,ConstK,IdK} ExpKind;

typedef enum {Void,Integer,Boolean} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode {
	struct treeNode* child[MAXCHILDREN];
	struct treeNode* sibling;
	int lineno;
	NodeKind nodekind;
	union { StmtKind stmt; ExpKind exp; } kind;
	union { 
		TokenType op;
		int val;
		char* name;
	} attr;
	ExpType type;
} TreeNode;