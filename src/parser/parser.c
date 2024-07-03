#include <stdlib.h>
#include <assert.h>
#include "parser.h"

#ifndef INIT_POOL_SZ
#define INIT_POOL_SZ 128
#endif

#define consume() ({ Token consumed = parser->current; parser->current = parser->next; parser->next = lexer_next(parser->lexer); consumed; })
#define match(...) ({ bool matched = false; TokenKind to_match[] = {__VA_ARGS__}; for (size_t i = 0; i < sizeof(to_match)/sizeof(to_match[0]); ++i) if(to_match[i] == parser->current.kind) { matched = true; break; }; matched; })
#define grow() do {                                                                       \
        if ((double) parser->pool_size >= (double) parser->pool_capacity * 0.75) {        \
            parser->pool_capacity *= 2;                                                   \
            void *temp = realloc(parser->pool, sizeof(Expr) * parser->pool_capacity);    \
            if (temp == NULL) assert(false && "realloc failed");                          \
            parser->pool = (Expr *)temp;                                                 \
        }                                                                                 \
    } while (0)

Parser parser_init(Lexer *lexer) {
    Expr *pool = malloc(sizeof(Expr) * INIT_POOL_SZ);

    return (Parser) {
        .lexer = lexer,
        .pool_capacity = INIT_POOL_SZ,
        .pool_size = 0,
        .pool = pool,
        .current = lexer_next(lexer),
        .next = lexer_next(lexer),
    };
}

Expr *parser_parse_expr(Parser *parser) {
    return parser_parse_expr_comparison(parser);
}

Expr *parser_parse_expr_comparison(Parser *parser) {
    grow();

    Expr *lhs = parser_parse_expr_equality(parser);

    while (match(TOKEN_LT, TOKEN_GT, TOKEN_LTE, TOKEN_GTE)) {
        Token op = consume();
        Expr *rhs = parser_parse_expr_equality(parser);

        Expr *expr = &parser->pool[parser->pool_size++];
        expr->kind = EXPR_INFIX;
        expr->infix.op = op;
        expr->infix.lhs = lhs;
        expr->infix.rhs = rhs;

        lhs = expr;
    }

    return lhs;
}

Expr *parser_parse_expr_equality(Parser *parser) {
    grow();

    Expr *lhs = parser_parse_expr_factor(parser);

    while (match(TOKEN_EQ, TOKEN_NEQ)) {
        Token op = consume();
        Expr *rhs = parser_parse_expr_factor(parser);

        Expr *expr = &parser->pool[parser->pool_size++];
        expr->kind = EXPR_INFIX;
        expr->infix.op = op;
        expr->infix.lhs = lhs;
        expr->infix.rhs = rhs;

        lhs = expr;
    }

    return lhs;
}

Expr *parser_parse_expr_factor(Parser *parser) {
    grow();

    Expr *lhs = parser_parse_expr_term(parser);

    while (match(TOKEN_STAR, TOKEN_SLASH, TOKEN_PERCENT)) {
        Token op = consume();
        Expr *rhs = parser_parse_expr_term(parser);

        Expr *expr = &parser->pool[parser->pool_size++];
        expr->kind = EXPR_INFIX;
        expr->infix.op = op;
        expr->infix.lhs = lhs;
        expr->infix.rhs = rhs;

        lhs = expr;
    }

    return lhs;
}

Expr *parser_parse_expr_term(Parser *parser) {
    grow();

    Expr *lhs = parser_parse_expr_unary(parser);

    while (match(TOKEN_PLUS, TOKEN_MINUS)) {
        Token op = consume();
        Expr *rhs = parser_parse_expr_unary(parser);

        Expr *expr = &parser->pool[parser->pool_size++];
        expr->kind = EXPR_INFIX;
        expr->infix.op = op;
        expr->infix.lhs = lhs;
        expr->infix.rhs = rhs;

        lhs = expr;
    }

    return lhs;
}

Expr *parser_parse_expr_unary(Parser *parser) {
    grow();

    if (match(TOKEN_PLUS, TOKEN_MINUS)) {
        Token op = consume();
        Expr *rhs = parser_parse_expr_unary(parser);

        Expr *expr = &parser->pool[parser->pool_size++];
        expr->kind = EXPR_PREFIX;
        expr->prefix.op = op;
        expr->prefix.rhs = rhs;

        return expr;
    }

    return parser_parse_expr_primary(parser);
}

Expr *parser_parse_expr_primary(Parser *parser) {
    grow();

    if (match(TOKEN_NUMBER)) {
        Token token = consume();

        Expr *expr = &parser->pool[parser->pool_size++];
        expr->kind = EXPR_NUMBER;
        expr->number.token = token;

        return expr;
    }

    if (match(TOKEN_LPAREN)) {
        consume();
        Expr *expr = parser_parse_expr(parser);
//        parser_expect(parser, TOKEN_RPAREN);
        consume();

        return expr;
    }

    return NULL;
}


void parser_free(Parser *parser) {
    free(parser->pool);
    lexer_free(parser->lexer);
}