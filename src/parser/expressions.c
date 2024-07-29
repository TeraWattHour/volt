#include <errno.h>
#include <stdio.h>
#include "parser.h"
#include "parser_macros.h"

Expr *comparison(Parser *parser);
Expr *equality(Parser *parser);
Expr *multiplicative(Parser *parser);
Expr *additive(Parser *parser);
Expr *unary(Parser *parser);
Expr *primary(Parser *parser);

Expr *parser_expression(Parser *parser) {
    return comparison(parser);
}

Expr *comparison(Parser *parser) {
    Expr *lhs = equality(parser);

    while (match(TOKEN_LT, TOKEN_GT, TOKEN_LTE, TOKEN_GTE)) {
        Token op = consume();
        Expr *rhs = equality(parser);

        Expr *expr = pool_alloc(expr);
        expr->kind = EXPR_INFIX;
        expr->infix.op = op;
        expr->infix.lhs = lhs;
        expr->infix.rhs = rhs;

        lhs = expr;
    }

    return lhs;
}

Expr *equality(Parser *parser) {
    Expr *lhs = additive(parser);

    while (match(TOKEN_EQ, TOKEN_NEQ)) {
        Token op = consume();
        Expr *rhs = additive(parser);

        Expr *expr = pool_alloc(expr);
        expr->kind = EXPR_INFIX;
        expr->infix.op = op;
        expr->infix.lhs = lhs;
        expr->infix.rhs = rhs;

        lhs = expr;
    }

    return lhs;
}

Expr *additive(Parser *parser) {
    Expr *lhs = multiplicative(parser);

    while (match(TOKEN_PLUS, TOKEN_MINUS)) {
        Token op = consume();
        Expr *rhs = multiplicative(parser);

        Expr *expr = pool_alloc(expr);
        expr->kind = EXPR_INFIX;
        expr->infix.op = op;
        expr->infix.lhs = lhs;
        expr->infix.rhs = rhs;

        lhs = expr;
    }

    return lhs;
}

Expr *multiplicative(Parser *parser) {
    Expr *lhs = unary(parser);

    while (match(TOKEN_STAR, TOKEN_SLASH, TOKEN_PERCENT)) {
        Token op = consume();
        Expr *rhs = unary(parser);

        Expr *expr = pool_alloc(expr);
        expr->kind = EXPR_INFIX;
        expr->infix.op = op;
        expr->infix.lhs = lhs;
        expr->infix.rhs = rhs;

        lhs = expr;
    }

    return lhs;
}

Expr *unary(Parser *parser) {
    if (match(TOKEN_MINUS, TOKEN_INCREMENT, TOKEN_DECREMENT)) {
        Token op = consume();
        Expr *rhs = unary(parser);

        Expr *expr = pool_alloc(expr);
        expr->kind = EXPR_PREFIX;
        expr->prefix.op = op;
        expr->prefix.rhs = rhs;

        return expr;
    }

    return primary(parser);
}

Expr *primary(Parser *parser) {
    if (match(TOKEN_NUMBER)) {
        Token token = consume();

        Expr *expr = pool_alloc(expr);
        expr->kind = EXPR_NUMBER;
        expr->number.token = token;

        char *end = token.end;
        errno = 0;
        if (token.numeric_literal.is_float) {
            double d = strtod(token.start, &end);
            if ((d == 0 && errno != 0) || (end == token.start))
                assert(false && "strtod failed");

            expr->number.value.f = d;
        } else {
            int radix = token.numeric_literal.radix;
            char *start = token.start;

            if (radix == 8) start += 2;

            int64_t l = strtoll(start, &end, radix);
            if ((l == 0 && errno != 0) || (end == start))
                assert(false && "strtoll failed");

            expr->number.value.i = l;
        }

        return expr;
    }

    if (match(TOKEN_RUNE)) {
        Token token = consume();

        assert(0 && "UNIMPLEMENTED");
    }

    if (match(TOKEN_LPAREN)) {
        consume();
        Expr *expr = parser_expression(parser);
        if (!parser_expect(parser, TOKEN_RPAREN)) {
            fprintf(stderr, "Expected ')' at %zu:%zu\n", parser->current.line, parser->current.column);
            exit(1);
        }

        return expr;
    }

    return NULL;
}
