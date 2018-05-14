#pragma once
#include "globals.h"
#include "lexer.h"

typedef struct {
	Lexer* lexer;
	int error;
	TokenType token;
	TokenType lastToken;
} Parser;

void parser_init(Parser*,Lexer*);

TreeNode* parser_parse(Parser*);