#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "casa_token.h"
#include "lexer.h"

int is_single_symbol(const char* s) { return s && strlen(s) == 1; }

static void Tipo();
static void VarDecl();
static void VarPart();
static void Statement();
static void StmtSeq();
static void Composto();
static void Variavel();
static void Fatores();
static void Termo();
static void SimpleExpr();
static void Relacao();
static void Expr();

void CasaToken(const char* expected) {
    Token* t = lexer_getCurrent();
    if (!t) {
        fprintf(stderr, "1:fim de arquivo não esperado.\n");
        exit(1);
    }

    if (strcmp(t->type, TOKEN_EOF) == 0) {
        fprintf(stderr, "%d:fim de arquivo não esperado.\n", t->line);
        exit(1);
    }

    int match = 0;
    if (is_single_symbol(expected)) {
        if (strcmp(t->lexeme, expected) == 0) match = 1;
    } else {
        if (strcmp(t->type, expected) == 0)
            match = 1;
        else if (strcmp(t->lexeme, expected) == 0)
            match = 1;  // aceitar lexema também se for passado
    }

    if (!match) {
        fprintf(stderr, "%d:token nao esperado [%s].\n", t->line, t->lexeme);
        exit(1);
    }

    /* consome token */
    lexer_next();
}

void parseProgram() {
    CasaToken("KW_PROGRAM");
    CasaToken("ID");
    CasaToken(";");

    printf("<programa> ::= program <identificador>; <bloco>.\n");
    printf(
        "<bloco> ::= <parte de declaracoes de variaveis> <comando composto>\n");

    Token* t = lexer_getCurrent();

    if (t && strcmp(t->type, "KW_VAR") == 0) {
        VarPart();
    }

    Composto();
    CasaToken(".");

    Token* cur = lexer_getCurrent();
    if (cur && strcmp(cur->type, TOKEN_EOF) != 0) {
        fprintf(stderr, "%d:token nao esperado [%s].\n", cur->line,
                cur->lexeme);
        exit(1);
    }
}

static void VarPart() {
    CasaToken("KW_VAR");
    printf(
        "<parte de declaracoes de variaveis> ::= { var <declaracao de "
        "variaveis> {; <declaracao de variaveis>}; }\n");
    while (1) {
        Token* t = lexer_getCurrent();
        if (t && strcmp(t->type, "ID") == 0) {
            VarDecl();
        } else
            break;
    }
}

static void VarDecl() {
    printf(
        "<declaracao de variaveis> ::= <lista de identificadores> : <tipo>\n");
    printf(
        "<lista de identificadores> ::= <identificador> {, "
        "<identificador>}\n");
    CasaToken("ID");
    Token* t = lexer_getCurrent();
    while (t && strcmp(t->lexeme, ",") == 0) {
        CasaToken(",");
        CasaToken("ID");
        t = lexer_getCurrent();
    }
    CasaToken(":");
    Tipo();
    CasaToken(";");
}

static void Tipo() {
    Token* t = lexer_getCurrent();
    printf("<tipo> ::= integer | real\n");
    if (!t) {
        fprintf(stderr, "1:fim de arquivo não esperado.\n");
        exit(1);
    }
    if (strcmp(t->type, "KW_INTEGER") == 0) {
        CasaToken("KW_INTEGER");
    } else if (strcmp(t->type, "KW_REAL") == 0) {
        CasaToken("KW_REAL");
    } else {
        fprintf(stderr, "%d:token nao esperado [%s].\n", t->line, t->lexeme);
        exit(1);
    }
}

static void Composto() {
    printf("<comando composto> ::= begin <comando> ; { <comando> ; } end\n");
    CasaToken("KW_BEGIN");
    StmtSeq();
    CasaToken("KW_END");
}

// uma sequencia de comandos seguidos
static void StmtSeq() {
    Statement();
    Token* t = lexer_getCurrent();
    while (t && strcmp(t->lexeme, ";") == 0) {
        CasaToken(";");
        Statement();
        t = lexer_getCurrent();
    }
}

static void Statement() {
    Token* t = lexer_getCurrent();
    if (!t) return;

    printf(
        "<comando> ::= <atribuicao> | <comando composto> | <comando "
        "condicional> | <comando repetitivo>\n");

    if (strcmp(t->type, "ID") == 0) {
        printf("<atribuicao> ::= <variavel> := <expressao>\n");
        Variavel();
        CasaToken("OP_ASS");
        Expr();
    } else if (strcmp(t->type, "KW_BEGIN") == 0) {
        Composto();
    } else if (strcmp(t->type, "KW_IF") == 0) {
        printf(
            "<comando condicional> ::= if <expressao> then <comando> [ else "
            "<comando> ]\n");
        CasaToken("KW_IF");
        Expr();
        CasaToken("KW_THEN");
        Statement();
        Token* nx = lexer_getCurrent();
        if (nx && strcmp(nx->type, "KW_ELSE") == 0) {
            CasaToken("KW_ELSE");
            Statement();
        }
    } else if (strcmp(t->type, "KW_WHILE") == 0) {
        printf("<comando repetitivo> ::= while <expressao> do <comando>\n");
        CasaToken("KW_WHILE");
        Expr();
        CasaToken("KW_DO");
        Statement();
    }
}

static void Variavel() {
    printf("<variavel> ::= <identificador>\n");
    CasaToken("ID");
}

// precisei separar a expressao em termo e fatores pra funcionar direito
static void Expr() {
    printf(
        "<expressao> ::= <expressao simples> [ <relacao> <expressao simples> "
        "]\n");
    SimpleExpr();

    Token* t = lexer_getCurrent();
    if (t && (strcmp(t->type, "OP_EQ") == 0 || strcmp(t->type, "OP_NE") == 0 ||
              strcmp(t->type, "OP_LT") == 0 || strcmp(t->type, "OP_LE") == 0 ||
              strcmp(t->type, "OP_GE") == 0 || strcmp(t->type, "OP_GT") == 0)) {
        Relacao();
        SimpleExpr();
    }
}

static void SimpleExpr() {
    printf("<expressao simples> ::= [ + | - ] <termo> { ( + | - ) <termo> }\n");
    Termo();

    while (1) {
        Token* t = lexer_getCurrent();
        if (!t) return;

        if (strcmp(t->type, "OP_AD") == 0) {
            CasaToken("OP_AD");
            Termo();
        } else if (strcmp(t->type, "OP_MIN") == 0) {
            CasaToken("OP_MIN");
            Termo();
        } else {
            break;
        }
    }
}

static void Termo() {
    printf("<termo> ::= <fator> { ( * | / ) <fator> }\n");
    Fatores();

    while (1) {
        Token* t = lexer_getCurrent();
        if (!t) return;

        if (strcmp(t->type, "OP_MUL") == 0) {
            CasaToken("OP_MUL");
            Fatores();
        } else if (strcmp(t->type, "OP_DIV") == 0) {
            CasaToken("OP_DIV");
            Fatores();
        } else {
            break;
        }
    }
}

static void Fatores() {
    Token* t = lexer_getCurrent();
    printf("<fator> ::= <variavel> | <numero> | ( <expressao> )\n");

    if (!t) {
        fprintf(stderr, "1:fim de arquivo não esperado.\n");
        exit(1);
    }

    if (strcmp(t->type, "ID") == 0) {
        Variavel();
    } else if (strcmp(t->type, "NUM_INT") == 0) {
        CasaToken("NUM_INT");
    } else if (strcmp(t->type, "NUM_REAL") == 0) {
        CasaToken("NUM_REAL");
    } else if (strcmp(t->lexeme, "(") == 0) {
        CasaToken("(");
        Expr();
        CasaToken(")");
    } else {
        fprintf(stderr, "%d:token nao esperado [%s].\n", t->line, t->lexeme);
        exit(1);
    }
}

static void Relacao() {
    Token* t = lexer_getCurrent();
    printf("<relacao> ::= = | <> | < | <= | >= | >\n");

    if (!t) {
        fprintf(stderr, "1:fim de arquivo não esperado.\n");
        exit(1);
    }

    if (strcmp(t->type, "OP_EQ") == 0) {
        CasaToken("OP_EQ");
    } else if (strcmp(t->type, "OP_NE") == 0) {
        CasaToken("OP_NE");
    } else if (strcmp(t->type, "OP_LT") == 0) {
        CasaToken("OP_LT");
    } else if (strcmp(t->type, "OP_LE") == 0) {
        CasaToken("OP_LE");
    } else if (strcmp(t->type, "OP_GE") == 0) {
        CasaToken("OP_GE");
    } else if (strcmp(t->type, "OP_GT") == 0) {
        CasaToken("OP_GT");
    } else {
        fprintf(stderr, "%d:token nao esperado [%s].\n", t->line, t->lexeme);
        exit(1);
    }
}
