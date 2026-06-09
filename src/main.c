#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "utils.h"
#include "symbols.h"
#include "casa_token.h"

int main(int argc, char** argv) {
    char* input = readFile(argv[1]);
    if (!input) {
        printf("Erro lendo arquivo\n");
        return 1;
    }

    FILE* lexFile = fopen("output.lex", "w");
    FILE* tsFile = fopen("output.ts", "w");
    FILE* errFile = fopen("output.err", "w");

    initSymbolTable();

    /* tokeniza e inicializa stream para o parser */
    lexer_init(input, lexFile, errFile);

    /* lê o primeiro token do programa */
    lexer_next();

    /* chamada ao símbolo inicial */
    parseProgram();

    saveSymbolTable(tsFile);

    fclose(lexFile);
    fclose(tsFile);
    fclose(errFile);
    free(input);

    printf("Pronto\n");
    return 0;
}
