#include "parser.h"
#include "util.h"

static TreeNode* stmt_sequence(Parser* parser);
static TreeNode* statement(Parser* parser);
static TreeNode* if_stmt(Parser* parser);
//static TreeNode* for_stmt(Parser* parser);
static TreeNode* while_stmt(Parser* parser);
static TreeNode* dowhile_stmt(Parser* parser);
static TreeNode* assign_stmt(Parser* parser);
static TreeNode* read_stmt(Parser* parser);
static TreeNode* write_stmt(Parser* parser);
static TreeNode* exp(Parser* parser);
static TreeNode* simple_exp(Parser* parser);
static TreeNode* term(Parser* parser);
static TreeNode* factor(Parser* parser);

static void syntaxError(Parser* parser,char* message) {
	printf("\n>>> ");
	printf("Syntax error at line %d : %s", parser->lexer->lineno, message);
	parser->error = TRUE;
}

static void advance(Parser* parser) {
	parser->lastToken = parser->token;
	parser->token = lexer_getToken(parser->lexer);
}

static void match(Parser* parser, TokenType expected) {
	if (parser->token == expected) {
		advance(parser);
	}
	else {
		syntaxError(parser, "unexpected token -> ");
		printToken(parser->token, parser->lexer->tokenString);
		printf("           ");
	}
}

TreeNode* stmt_sequence(Parser* parser) {
	TreeNode* t = statement(parser);
	TreeNode* p = t;
	while (parser->token != ENDFILE) {
		TreeNode* q;
		if (parser->lastToken != RBRACE) {
			match(parser, SEMI);
		}
		if (parser->token == RBRACE || parser->token == ENDFILE) {
			break;
		}
		q = statement(parser);
		if (q != NULL) {
			if (t == NULL) {
				t = p = q;
			}
			else {
				p->sibling = q;
				p = q;
			}
		}
	}
	return t;
}

TreeNode* statement(Parser* parser) {
	TreeNode* t = NULL;
	switch (parser->token) {
	case IF:
		t = if_stmt(parser);
		break;
	case WHILE:
		t = while_stmt(parser);
		break;
	case DO:
		t = dowhile_stmt(parser);
		break;
	case ID:
		t = assign_stmt(parser);
		break;
	case READ:
		t = read_stmt(parser);
		break;
	case WRITE:
		t = write_stmt(parser);
		break;
	default:
		syntaxError(parser, "unexpected token -> ");
		printToken(parser->token, parser->lexer->tokenString);
		advance(parser);
		break;
	}
	return t;
}

TreeNode* if_stmt(Parser* parser) {
	TreeNode* t = newStmtNode(IfK, parser->lexer->lineno);
	match(parser,IF);
	match(parser, LPARAM);
	if (t != NULL) {
		t->child[0] = exp(parser);
	}
	match(parser, RPARAM);
	match(parser, LBRACE);
	if (t != NULL) {
		t->child[1] = stmt_sequence(parser);
	}
	match(parser, RBRACE);
	if (parser->token == ELSE) {
		match(parser, ELSE);
		match(parser, LBRACE);
		if (t != NULL) {
			t->child[2] = stmt_sequence(parser);
		}
		match(parser, RBRACE);
	}
	return t;
}

//static TreeNode* for_stmt(Parser* parser) {
//	TreeNode* t = newStmtNode(ForK, parser->lexer->lineno);
//	match(parser, FOR);
//	match(parser, LPARAM);
//	if (t != NULL) {
//		t->child[0] = exp(parser);
//	}
//	match(parser, RPARAM);
//}

static TreeNode* while_stmt(Parser* parser)
{
	TreeNode* t = newStmtNode(WhileK, parser->lexer->lineno);
	match(parser, WHILE);
	match(parser, LPARAM);
	if (t != NULL) {
		t->child[0] = exp(parser);
	}
	match(parser, RPARAM);
	match(parser, LBRACE);
	if (t != NULL) {
		t->child[1] = stmt_sequence(parser);
	}
	match(parser, RBRACE);
	return t;
}

static TreeNode* dowhile_stmt(Parser* parser)
{
	TreeNode* t = newStmtNode(DoWhileK, parser->lexer->lineno);
	match(parser, DO);
	match(parser, LBRACE);
	if (t != NULL) {
		t->child[0] = stmt_sequence(parser);
	}
	match(parser, RBRACE);
	match(parser, WHILE);
	match(parser, LPARAM);
	if (t != NULL) {
		t->child[1] = exp(parser);
	}
	match(parser, RPARAM);
}
TreeNode* assign_stmt(Parser* parser) {
	TreeNode* t = newStmtNode(AssignK, parser->lexer->lineno);
	if (t != NULL && parser->token == ID) {
		t->attr.name = copyString(parser->lexer->tokenString);
	}
	match(parser, ID);
	match(parser, ASSIGN);
	if (t != NULL) {
		t->child[0] = exp(parser);
	}
	return t;
}

TreeNode* read_stmt(Parser* parser) {
	TreeNode* t = newStmtNode(ReadK, parser->lexer->lineno);
	match(parser,READ);
	if (t != NULL && parser->token == ID) {
		t->attr.name = copyString(parser->lexer->tokenString);
	}
	match(parser,ID);
	return t;
}

TreeNode* write_stmt(Parser* parser) {
	TreeNode* t = newStmtNode(WriteK, parser->lexer->lineno);
	match(parser, WRITE);
	if (t != NULL) {
		t->child[0] = exp(parser);
	}
	return t;
}

TreeNode* exp(Parser* parser) {
	TreeNode* t = simple_exp(parser);
	if (parser->token == LT || parser->token == EQ || parser->token == GT) {
		TreeNode* p = newExpNode(OpK, parser->lexer->lineno);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = parser->token;
			t = p;
		}
		match(parser, parser->token);
		if (t != NULL) {
			t->child[1] = simple_exp(parser);
		}
	}
	return t;
}

TreeNode* simple_exp(Parser* parser) {
	TreeNode* t = term(parser);
	while (parser->token == PLUS || parser->token == MINUS) {
		TreeNode* p = newExpNode(OpK, parser->lexer->lineno);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = parser->token;
			t = p;
			match(parser, parser->token);
			t->child[1] = term(parser);
		}
	}
	return t;
}

TreeNode* term(Parser* parser) {
	TreeNode* t = factor(parser);
	while (parser->token == TIMES || parser->token == OVER) {
		TreeNode* p = newExpNode(OpK, parser->lexer->lineno);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = parser->token;
			t = p;
			match(parser, parser->token);
			p->child[1] = factor(parser);
		}
	}
	return t;
}

TreeNode* factor(Parser* parser) {
	TreeNode* t = NULL;
	switch (parser->token) {
	case NUM:
		t = newExpNode(ConstK, parser->lexer->lineno);
		if (t != NULL && parser->token == NUM) {
			t->attr.val = atoi(parser->lexer->tokenString);
		}
		match(parser,NUM);
		break;
	case ID:
		t = newExpNode(IdK, parser->lexer->lineno);
		if (t != NULL && parser->token == ID) {
			t->attr.name = copyString(parser->lexer->tokenString);
		}
		match(parser, ID);
		break;
	case LPARAM:
		match(parser, LPARAM);
		t = exp(parser);
		match(parser, RPARAM);
		break;
	default:
		syntaxError(parser,"unexpected token -> ");
		printToken(parser->token, parser->lexer->tokenString);
		advance(parser);
		break;
	}
	return t;
}

void parser_init(Parser* parser,Lexer* lexer) {
	parser->lexer = lexer;
	parser->error = FALSE;
}

TreeNode* parser_parse(Parser* parser) {
	TreeNode* t;
	advance(parser);
	t = stmt_sequence(parser);
	if (parser->token != ENDFILE) {
		syntaxError(parser,"code ends before file");
	}
	return t;
}