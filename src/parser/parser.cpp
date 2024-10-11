#include <stdlib.h>

#include "parser.h"
#include "parser_macros.h"

std::unique_ptr<Block> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> stmts;
    
    while (current.kind != TokenKind::RBRACE && current.kind != TokenKind::_EOF) {
        stmts.push_back(statement());
    }
    
    if (current.kind != TokenKind::RBRACE) throw std::runtime_error("sdjsiadjopas");
        
    advance();
    
    return std::make_unique<Block>(stmts);
}

std::unique_ptr<Stmt> Parser::statement() {
    using enum TokenKind;
    switch (current.kind) {
        case LET: return let();
        default: return expression_statement();
    }
}

std::unique_ptr<Stmt> Parser::expression_statement() {
    auto expr = expression();
    auto returned = eat(TokenKind::SEMICOLON);
    
    return std::make_unique<ExpressionStatement>(expr, !returned);
}

std::unique_ptr<Stmt> Parser::let() {
    advance();
    
    Token name = advance();
    if (name.kind != TokenKind::IDENTIFIER) {
        throw std::runtime_error("Expected identifier after `let`\n");
    }
    
    auto let = std::make_unique<Let>(name);
    
    if (eat(TokenKind::ASSIGN)) let->set_value(expression());
    else if (eat(TokenKind::LBRACE)) let->set_value(block());
    else throw std::runtime_error("expected `=` or `{` after `let`\n");
        
    if (!eat(TokenKind::SEMICOLON)) {
        throw std::runtime_error("Expected `;` after `let` statement\n");
    }
    
    return let;
}
