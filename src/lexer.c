#include "lexer.h"
#include "symbols.h"
#include "utils.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define MAX 100

// static pra limitar o scopo :)
static Token tokens[10000];
static int token_count = 0;
static int current_index = -1;

// isso podia ficar mais bonito
static void push_token(const char* t, const char* l, int line, int col) {
    if (token_count >= (int)(sizeof(tokens) / sizeof(tokens[0]))) return;
    strncpy(tokens[token_count].type, t, sizeof(tokens[token_count].type) - 1);
    tokens[token_count].type[sizeof(tokens[token_count].type) - 1] = '\0';
    strncpy(tokens[token_count].lexeme, l,
            sizeof(tokens[token_count].lexeme) - 1);
    tokens[token_count].lexeme[sizeof(tokens[token_count].lexeme) - 1] = '\0';
    tokens[token_count].line = line;
    tokens[token_count].col = col;
    token_count++;
}

void lexer_init(char* input, FILE* lexFile, FILE* errFile) {
    token_count = 0;
    current_index = -1;
    runLexer(input, lexFile, errFile);

    push_token(TOKEN_EOF, "",
               (token_count > 0 ? tokens[token_count - 1].line : 1), 0);
}

Token* lexer_getCurrent() {
    if (current_index < 0 || current_index >= token_count) return NULL;
    return &tokens[current_index];
}

void lexer_next() {
    if (current_index < token_count - 1) current_index++;
}

void writeToken(FILE* f, const char* t, const char* l, int line, int col) {
    fprintf(f, "<%s, %s> %d %d\n", t, l, line, col);
    push_token(t, l, line, col);
}

void writeError(FILE* f, const char* msg, int line, int col) {
    fprintf(f, "Erro: %s em %d:%d\n", msg, line, col);
}

void runLexer(char* input, FILE* lexFile, FILE* errFile) {
    int i = 0, line = 1, col = 1;

    while (input[i] != '\0') {
        // espaços
        if (input[i] == ' ' || input[i] == '\t') {
            i++;
            col++;
            continue;
        }
        if (input[i] == '\n') {
            i++;
            line++;
            col = 1;
            continue;
        }

        // comentarios
        if (input[i] == '{') {
            i++;
            col++;
            while (input[i] != '}' && input[i] != '\0') {
                if (input[i] == '\n') {
                    line++;
                    col = 1;
                } else
                    col++;
                i++;
            }
            if (input[i] == '}') {
                i++;
                col++;
            } else {
                writeError(errFile, "Comentario não fechado", line, col);
            }
            continue;
        }

        // identificadores e keywords
        if (isalpha(input[i])) {
            int start = i, start_col = col;

            while (isalnum(input[i])) {
                i++;
                col++;
            }

            char lexeme[MAX];
            int len = i - start;
            strncpy(lexeme, &input[start], len);
            lexeme[len] = '\0';

            const char* token = getKeywordToken(lexeme);

            writeToken(lexFile, token, lexeme, line, start_col);

            if (strcmp(token, "ID") == 0)
                addSymbol(lexeme, "ID");
            else
                addSymbol(lexeme, "KW");

            continue;
        }

        // nnumeros
        if (isdigit(input[i])) {
            int start = i, start_col = col;
            int isReal = 0;

            while (isdigit(input[i])) {
                i++;
                col++;
            }

            if (input[i] == '.') {
                isReal = 1;
                i++;
                col++;
                while (isdigit(input[i])) {
                    i++;
                    col++;
                }
            }

            char lexeme[MAX];
            int len = i - start;
            strncpy(lexeme, &input[start], len);
            lexeme[len] = '\0';

            writeToken(lexFile, isReal ? "NUM_REAL" : "NUM_INT", lexeme, line,
                       start_col);

            continue;
        }

        // operadores
        if (input[i] == ':' && input[i + 1] == '=') {
            writeToken(lexFile, "OP_ASS", ":=", line, col);
            i += 2;
            col += 2;
            continue;
        }

        if (input[i] == '<') {
            if (input[i + 1] == '=') {
                writeToken(lexFile, "OP_LE", "<=", line, col);
                i += 2;
                col += 2;
            } else if (input[i + 1] == '>') {
                writeToken(lexFile, "OP_NE", "<>", line, col);
                i += 2;
                col += 2;
            } else {
                writeToken(lexFile, "OP_LT", "<", line, col);
                i++;
                col++;
            }
            continue;
        }

        if (input[i] == '>') {
            if (input[i + 1] == '=') {
                writeToken(lexFile, "OP_GE", ">=", line, col);
                i += 2;
                col += 2;
            } else {
                writeToken(lexFile, "OP_GT", ">", line, col);
                i++;
                col++;
            }
            continue;
        }

        // operadores com termos e fatores
        if (input[i] == '+') {
            writeToken(lexFile, "OP_AD", "+", line, col);
            i++;
            col++;
            continue;
        }

        if (input[i] == '-') {
            writeToken(lexFile, "OP_MIN", "-", line, col);
            i++;
            col++;
            continue;
        }

        if (input[i] == '*') {
            writeToken(lexFile, "OP_MUL", "*", line, col);
            i++;
            col++;
            continue;
        }

        if (input[i] == '/') {
            writeToken(lexFile, "OP_DIV", "/", line, col);
            i++;
            col++;
            continue;
        }

        // simbolos
        char c = input[i];

        if (strchr(";,().:E+-*/", c)) {
            char lex[2] = {c, '\0'};
            writeToken(lexFile, "SYMBOL", lex, line, col);
            i++;
            col++;
            continue;
        }

        writeError(errFile, "Caractere invalido", line, col);
        i++;
        col++;
    }
}
