#pragma once

#include "lexer.h"

class Stmt {
public:
    virtual ~Stmt() = default;
    
    virtual void print(std::ostream& os) const = 0;
    
    friend std::ostream &operator<<(std::ostream &os, const Stmt &stmt) {
        stmt.print(os);
        return os;
    }
};

class Expr : public Stmt {
public:
    virtual ~Expr() = default;
    
    virtual void print(std::ostream& os) const = 0;
    
    friend std::ostream &operator<<(std::ostream &os, const Expr &expr) {
        expr.print(os);
        return os;
    }
};

#include "expressions.h"
#include "statements.h"

class ParserError;

class Parser {
public:
    Parser(Lexer &lexer) : mLexer(lexer) {
        current = lexer.next();
        next = lexer.next();
    }
    
    std::vector<std::unique_ptr<Stmt>> parse() {
        std::vector<std::unique_ptr<Stmt>> stmts;
        
        while (current.kind != TokenKind::_EOF) {
            stmts.push_back(statement());
        }
        
        return stmts;
    }
    
private:
    Token current;
    Token next;
    Lexer &mLexer;
    
    std::unique_ptr<Stmt> statement();
    
    std::unique_ptr<Block> block();
    
    std::unique_ptr<Stmt> let();
    std::unique_ptr<Stmt> expression_statement();
    
    std::unique_ptr<Expr> expression();
    
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> equality();
    std::unique_ptr<Expr> multiplicative();
    std::unique_ptr<Expr> additive();
    std::unique_ptr<Expr> prefix();
    std::unique_ptr<Expr> cast();
    std::unique_ptr<Expr> primary();
    
    bool eat(TokenKind kind) {
        if (current.kind == kind) {
            advance();
            return true;
        }
        
        return false;
    }
    
    Token advance() {
        Token consumed = this->current;
        this->current = this->next;
        this->next = mLexer.next();
        
        return consumed;
    }
    
    friend ParserError;
};

class ParserError : public std::exception {
public:
    ParserError(Parser *parser, Token &token, std::string message) : mMessage(message), mToken(token), mFilename(parser->mLexer.mFilename) {};

    const char *what() const noexcept {
        static std::string formattedMessage;
        std::ostringstream oss;
        oss << "ERROR " << mFilename << ":" << mToken.line << ":" << mToken.column << ": " << mMessage;
        formattedMessage = oss.str();
        return formattedMessage.c_str();
    }

private:
    std::string mMessage;
    std::string mFilename;
    Token mToken;
};
