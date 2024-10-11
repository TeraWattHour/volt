#include "parser.h"
#include "parser_macros.h"

int number_base(std::string literal) {
    if (literal.length() <= 2) return 10;
    
    switch (literal[1]) {
        case 'b': return 2;
        case 'x': return 16;
        case 'o': return 8;
        default: return 10;
    }
}

std::unique_ptr<Expr> Parser::expression() {
    return comparison();
};

std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> lhs = equality();
    
    using enum TokenKind;
    while (match(LT, GT, LTE, GTE)) {
        Token op = advance();
    
        lhs = std::make_unique<Infix>(op, std::move(lhs), equality());
    }

    return lhs;
}

std::unique_ptr<Expr> Parser::equality() {
    std::unique_ptr<Expr> lhs = additive();

    using enum TokenKind;
    while (match(EQ, NEQ)) {
        Token op = advance();

        lhs = std::make_unique<Infix>(op, std::move(lhs), additive());
    }

    return lhs;
}

std::unique_ptr<Expr> Parser::additive() {
    std::unique_ptr<Expr> lhs = multiplicative();

    using enum TokenKind;
    while (match(PLUS, MINUS)) {
        Token op = advance();
        
        lhs = std::make_unique<Infix>(op, std::move(lhs), multiplicative());
    }

    return lhs;
}

std::unique_ptr<Expr> Parser::multiplicative() {
    std::unique_ptr<Expr> lhs = prefix();

    using enum TokenKind;
    while (match(STAR, SLASH, PERCENT)) {
        Token op = advance();
        
        lhs = std::make_unique<Infix>(op, std::move(lhs), prefix());
    }

    return lhs;
}

std::unique_ptr<Expr> Parser::prefix() {
    using enum TokenKind;
    if (match(MINUS, INCREMENT, DECREMENT)) {
        Token op = advance();
        
        return std::make_unique<Prefix>(op, prefix());
    }

    return cast();
}

std::unique_ptr<Expr> Parser::cast() {
    std::unique_ptr<Expr> lhs = primary();

    using enum TokenKind;
    if (match(COLON_COLON)) {
        Token castOp = advance();

        if (!match(IDENTIFIER)) {
//            throw ParserError(this, peek(), "Expected a type after '::'");
        }

        Token typeToken = advance();

        // Create a cast expression (lhs::type)
        return std::make_unique<Cast>(castOp, lhs, typeToken);
    }

    // If no cast operator is found, return the original expression
    return lhs;
}


std::unique_ptr<Expr> Parser::primary() {
    using enum TokenKind;
    
    if (match(NUMBER)) {
        Token token = advance();
        auto literal = std::string(token.start, token.end - token.start);
        int base = number_base(literal);
        bool is_float = literal.find('.') != std::string::npos || literal.find('e') != std::string::npos;
        
        literal.erase(std::remove(literal.begin(), literal.end(), '_'), literal.end());
        
        try {
            if (is_float) return std::make_unique<Double>(token, std::stod(literal));
            return std::make_unique<Integer>(token, std::stoll(base == 10 ? literal : literal.substr(2), nullptr, base));
        } catch (std::out_of_range const &e) {
            throw ParserError(this, token, "numeric literal out of bounds");
        } catch (std::invalid_argument const &e) {
            throw ParserError(this, token, std::format("invalid numeric literal ({})", e.what()));
        }
    }

//    if (match(TOKEN_RUNE)) {
//        Token token = consume();
//
//        assert(0 && "UNIMPLEMENTED");
//    }
//
    if (match(LPAREN)) {
        advance();
        auto expr = expression();
        if (!eat(RPAREN)) {
            throw ParserError(this, current, "expected ')'");
        }

        return expr;
    }

    throw std::runtime_error("unreachable");
}
