#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stdio.h>
void initSymbolTable();
void addSymbol(char *lexeme, char *type);
void saveSymbolTable(FILE *file);

#endif
