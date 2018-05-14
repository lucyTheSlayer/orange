#pragma once

#include "globals.h"

#define MAXTOKENLEN 40
#define BUFLEN 256



typedef struct{
	int lineno;
	char lineBuf[BUFLEN];
	int linepos;
	int bufsize;
	int EOF_FLAG;
	FILE* source;
	char tokenString[MAXTOKENLEN + 1];
} Lexer;

void lexer_init(Lexer* lexer,FILE* source);

TokenType lexer_getToken(Lexer*);