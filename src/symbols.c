#include <stdio.h>
#include <string.h>
#include <strings.h>

typedef struct {
  char lexeme[100];
  char type[20];
} Symbol;

static Symbol table[1000];
static int count = 0;

void initSymbolTable() { count = 0; }

int exists(const char *lexeme) {
  for (int i = 0; i < count; i++) {
    if (strcasecmp(table[i].lexeme, lexeme) == 0)
      return 1;
  }
  return 0;
}

void addSymbol(const char *lexeme, const char *type) {
  if (exists(lexeme))
    return;

  strcpy(table[count].lexeme, lexeme);
  strcpy(table[count].type, type);
  count++;
}

void saveSymbolTable(FILE *file) {
  for (int i = 0; i < count; i++) {
    fprintf(file, "%s %s\n", table[i].lexeme, table[i].type);
  }
}
