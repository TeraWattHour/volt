#include <stdlib.h>
#include "parser.h"
#include "parser_macros.h"
#include "ast/printer.h"

Parser parser_init(Lexer *lexer) {
    return (Parser) {
        .lexer = lexer,

        .expr_pool = malloc(sizeof(Expr) * INIT_POOL_SZ),
        .expr_pool_capacity = INIT_POOL_SZ,
        .expr_pool_size = 0,

        .stmt_pool = malloc(sizeof(Stmt) * INIT_POOL_SZ),
        .stmt_pool_capacity = INIT_POOL_SZ,
        .stmt_pool_size = 0,

        .current = lexer_next(lexer),
        .next = lexer_next(lexer),
    };
}

Stmt *parser_let(Parser *parser) {
    consume();

    Token name = consume();
    if (name.kind != TOKEN_IDENTIFIER) {
        ERROR("Expected identifier after `let`\n");
    }

    if (!parser_expect(parser, TOKEN_ASSIGN)) {
        ERROR("Expected `=` after `let`\n");
    }
    Expr *value = parser_expression(parser);
    if (!parser_expect(parser, TOKEN_SEMICOLON)) {
        ERROR("Expected `;` after `let` statement\n");
    }

    Stmt *stmt = pool_alloc(stmt);
    stmt->kind = TOKEN_LET;
    stmt->let.name = name;
    stmt->let.value = value;

    return stmt;
}

bool parser_expect(Parser *parser, TokenKind kind) {
    if (parser->current.kind == kind) {
        consume();
        return true;
    }

    return false;
}

void parser_free(Parser *parser) {
    free(parser->expr_pool);
    free(parser->stmt_pool);
    lexer_free(parser->lexer);
}