#include "lexer.h"

typedef enum {
	START,INASSIGN,INCOMMENT,INNUM,INID,DONE
} StateType;

void lexer_init(Lexer* lexer, FILE* source) {
	lexer->lineno = 0;
	lexer->bufsize = 0;
	lexer->linepos = 0;
	lexer->EOF_FLAG = FALSE;
	lexer->source = source;
}


static int getNextChar(Lexer* lexer) {
	if (lexer->linepos < lexer->bufsize) {
		return lexer->lineBuf[lexer->linepos++];
	}
	else {
		lexer->lineno++;
		if(fgets(lexer->lineBuf, BUFLEN - 1, lexer->source)){
			lexer->bufsize = strlen(lexer->lineBuf);
			lexer->linepos = 0;
			return lexer->lineBuf[lexer->linepos++];
		}
		else {
			lexer->EOF_FLAG = TRUE;
			return EOF;
		}
	}
}

static void ungetNextChar(Lexer* lexer) {
	if (!lexer->EOF_FLAG) {
		lexer->linepos--;
	}
}

static struct {
	char* str;
	TokenType tok;
} reservedWords[MAXRESERVED]
= {
	{"if",IF},{"else",ELSE},{"do",DO},{"for",FOR},{"while",WHILE},{"break",BREAK},{"continue",CONTINUE},{"read",READ},{"write",WRITE}
};

static TokenType reservedLookup(char* s) {
	int i;
	for (i = 0; i < MAXRESERVED; i++) {
		if (!strcmp(s, reservedWords[i].str)) {
			return reservedWords[i].tok;
		}
	}
	return ID;
}

TokenType lexer_getToken(Lexer* lexer) {
	int tokenStringIndex = 0;
	TokenType currentToken;
	StateType state = START;
	int save;
	while (state != DONE) {
		int c = getNextChar(lexer);
		save = TRUE;
		switch (state) {
		case START:
			if (isdigit(c)) {
				state = INNUM;
			}
			else if (isalpha(c)) {
				state = INID;
			}
			else if (c == ':') {
				state = INASSIGN;
			}
			else if ((c == ' ' || c == '\t' || c == '\n')) {
				save = FALSE;
			}
			else if (c == '#') {
				save = FALSE;
				state = INCOMMENT;
			}
			else {
				state = DONE;
				switch (c) {
				case EOF:
					save = FALSE;
					currentToken = ENDFILE;
					break;
				case '=':
					currentToken = EQ;
					break;
				case '<':
					currentToken = LT;
					break;
				case '>':
					currentToken = GT;
					break;
				case '+':
					currentToken = PLUS;
					break;
				case '-':
					currentToken = MINUS;
					break;
				case '*':
					currentToken = TIMES;
					break;
				case '/':
					currentToken = OVER;
					break;
				case '(':
					currentToken = LPARAM;
					break;
				case ')':
					currentToken = RPARAM;
					break;
				case ';':
					currentToken = SEMI;
					break;
				case '{':
					currentToken = LBRACE;
					break;
				case '}':
					currentToken = RBRACE;
					break;
				default:
					currentToken = ERROR;
					break;
				}
			}
			break;
		case INCOMMENT:
			save = FALSE;
			if (c == EOF) {
				state = DONE;
				currentToken = ENDFILE;
			}
			else if (c == '\n') {
				state = START;
			}
			break;
		case INASSIGN:
			state = DONE;
			if (c == '=') {
				currentToken = ASSIGN;
			}
			else {
				ungetNextChar(lexer);
				save = FALSE;
				currentToken = ERROR;
			}
			break;

		case INNUM:
			if (!isdigit(c)) {
				ungetNextChar(lexer);
				save = FALSE;
				state = DONE;
				currentToken = NUM;
			}
			break;
		case INID:
			if (!isalpha(c)) {
				ungetNextChar(lexer);
				save = FALSE;
				state = DONE;
				currentToken = ID;
			}
			break;
		case DONE:
		default:
			state = DONE;
			currentToken = ERROR;
			break;
		}
		if (save && tokenStringIndex <= MAXTOKENLEN) {
			lexer->tokenString[tokenStringIndex++] = (char)c;
		}
		if (state == DONE) {
			lexer->tokenString[tokenStringIndex] = '\0';
			if (currentToken == ID) {
				currentToken = reservedLookup(lexer->tokenString);
			}
		}
	}
	return currentToken;
}

