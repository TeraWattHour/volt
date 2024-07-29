#ifndef VOLT_PARSER_H
#define VOLT_PARSER_H

#include <stdint.h>
#include "lexer.h"

typedef struct Stmt {
    TokenKind kind;
    union {
        struct {
            Token name;
            struct Expr *value;
        } let;
    };
} Stmt;

typedef enum {
    EXPR_INFIX,
    EXPR_PREFIX,
    EXPR_SUFFIX,
    EXPR_NUMBER,
    EXPR_STRING,
    EXPR_RUNE,
    EXPR_CHAIN,
    EXPR_BOOL,
} ExprKind;

typedef struct Expr {
    ExprKind kind;
    union {
        struct {
            Token op;
            struct Expr *lhs;
            struct Expr *rhs;
        } infix;

        struct {
            Token op;
            struct Expr *rhs;
        } prefix;

        struct {
            Token op;
            struct Expr *lhs;
        } suffix;

        struct {
            Token token;
            union {
                int64_t i;
                double f;
            } value;
        } number;
    };
} Expr;

typedef struct {
    Lexer *lexer;

    Token current;
    Token next;

    Expr *expr_pool;
    size_t expr_pool_size;
    size_t expr_pool_capacity;

    Stmt *stmt_pool;
    size_t stmt_pool_size;
    size_t stmt_pool_capacity;
} Parser;


Parser parser_init(Lexer *lexer);
void parser_free(Parser *parser);
bool parser_expect(Parser *parser, TokenKind kind);

Stmt *parser_let(Parser *parser);
Expr *parser_expression(Parser *parser);


#endif //VOLT_PARSER_H
