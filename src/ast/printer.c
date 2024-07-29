#include <stdio.h>
#include "./printer.h"

void ast_print(Expr *expr, size_t indent) {
    switch (expr->kind) {
        case EXPR_INFIX:
            printf("%*sInfixExpr(%s)\n", (int)indent, "", token_kind_to_name(expr->infix.op.kind));
            ast_print(expr->infix.lhs, indent + 2);
            ast_print(expr->infix.rhs, indent + 2);
            break;
        case EXPR_PREFIX:
            printf("%*sPrefixExpr(%s)\n", (int)indent, "", token_kind_to_name(expr->prefix.op.kind));
            ast_print(expr->prefix.rhs, indent + 2);
            break;
        case EXPR_NUMBER:
            printf("%*sNumberExpr(%.*s)\n", (int)indent, "", (int)(expr->number.token.end - expr->number.token.start), expr->number.token.start);
            break;
    }
}