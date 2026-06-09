#ifndef LEXER_H
#define LEXER_H


#include <stdio.h>
typedef struct {
	char type[50];
	char lexeme[128];
	int line;
	int col;
} Token;

void runLexer(char *input, FILE *file, FILE *err);

/* Lexer token-stream API for the parser */
void lexer_init(char *input, FILE *lexFile, FILE *errFile);
Token *lexer_getCurrent();
void lexer_next();

#define TOKEN_EOF "EOF"

#endif
