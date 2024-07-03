#ifndef VOLT_PARSER_H
#define VOLT_PARSER_H

#include <stdint.h>
#include "lexer.h"

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

    Expr *pool;
    size_t pool_size;
    size_t pool_capacity;
} Parser;


Parser parser_init(Lexer *lexer);
void parser_free(Parser *parser);

Expr *parser_parse_expr(Parser *parser);


#endif //VOLT_PARSER_H
