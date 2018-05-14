#pragma once
#include "globals.h"
void printToken(TokenType, const char*);
TreeNode* newStmtNode(StmtKind,int);
TreeNode* newExpNode(ExpKind,int);
char* copyString(char*);
void printTree(TreeNode*);