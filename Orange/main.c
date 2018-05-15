#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "symtab.h"
#include "analyze.h"
#include "code.h"
#include "cgen.h"
#include "util.h"
int main() {
	FILE* source = fopen("hello.orange", "r");
	if (source == NULL) {
		fprintf(stderr, "source code not found");
		exit(1);
	}
	Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
	if (lexer == NULL) {
		exit(1);
	}
	lexer_init(lexer,source);
	/*TokenType tokenType;
	while ((tokenType = lexer_getToken(lexer)) != ENDFILE) {
		printToken(tokenType, lexer->tokenString);
	}*/
	Parser* parser = (Parser*)malloc(sizeof(Parser));
	parser_init(parser, lexer);
	TreeNode* syntaxTree = parser_parse(parser);
	printTree(syntaxTree);
	fclose(source);

	Symtab* symtab = (Symtab*)malloc(sizeof(Symtab));
	st_init(symtab);
	Analyzer* analyzer = (Analyzer*)malloc(sizeof(Analyzer));
	analyzer_init(analyzer, symtab);
	printf("\n Building Symbol Table... \n");
	buildSymtab(analyzer, syntaxTree);
	printf("\n Checking Types...\n");
	typeCheck(analyzer, syntaxTree);
	printf("\n Type Checking Finished");

	Coder* coder = (Coder*)malloc(sizeof(Coder));
	char* codefile = "hello.orangebc";
	FILE* out = fopen(codefile, "w");
	if (out == NULL) {
		exit(1);
	}
	coder_init(coder,out , symtab, TRUE);
	codeGen(coder, syntaxTree, codefile);
	fclose(out);

	char c;
	scanf_s("%c", &c);
	free(parser);
	free(lexer);
	free(symtab);
	free(analyzer);
	free(coder);
}