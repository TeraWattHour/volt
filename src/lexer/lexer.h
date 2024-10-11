#pragma once

#include <exception>
#include <format>
#include <iostream>
#include <sstream>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

class LexerError;

enum class TokenKind {
    _EOF,
    IDENTIFIER,
    NUMBER,
    STRING,
    RUNE,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    TRUE,
    FALSE,
    NIL,
    IF,
    ELIF,
    ELSE,
    FOR,

    LET,
    MUT,

    STRUCT,
    BREAK,
    CONTINUE,
    RETURN,
    MATCH,


    SEMICOLON,
    COLON_COLON,
    COLON,
    SCOPE,

    ASSIGN,
    GT,
    LT,
    LTE,
    GTE,
    NEQ,
    EQ,
    BANG,
    DOT,
    PLUS,
    INCREMENT,
    DECREMENT,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    AMPERSAND,
    PIPE,
    AND,
    OR,

    QUESTION,
};


namespace tokens {
using enum TokenKind;

static const std::unordered_map<std::string, TokenKind> operators = {
    {";", SEMICOLON},
    
    {"++", INCREMENT},
    {"+", PLUS},
    
    {"--", DECREMENT},
    {"-", MINUS},
    
    {"==", EQ},
    {"=", ASSIGN},
    
    {"!=", NEQ},
    
    {"<=", LTE},
    {"<", LT},
    
    {">=", GTE},
    {">", GT},
    
    {"::", COLON_COLON},
    {":", COLON},
    
    {"*", STAR},
    {"/", SLASH},
    {"%", PERCENT},
    
    {"&&", AND},
    {"&", AMPERSAND},
    
    {"||", OR},
    {"|", PIPE},
    
    {"(", LPAREN},
    {")", RPAREN},
    {"{", LBRACE},
    {"}", RBRACE},
    {"[", LBRACKET},
    {"]", RBRACKET},
    
    {"let", LET},
    {"mut", MUT},
    {"true", TRUE},
    {"false", FALSE},
};

std::string literal_for_operator(TokenKind op);
}

std::ostream &operator<<(std::ostream &os, const TokenKind &kind);

typedef struct Token {
    TokenKind kind;
    char *start;
    char *end;
    size_t line;
    size_t column;
} Token;

std::ostream &operator<<(std::ostream &os, const Token &token);

typedef struct {
    const TokenKind kind;
    char *keyword;
} KeywordMapping;

class Lexer {
public:
    const std::string mFilename;
    
    Lexer(std::string &filename, const char *source) : mFilename(filename), mSource(source), mSourceLength(strlen(source)), mCursor((char *) source) { };

    Token next();

private:
    char *mCursor;
    const char *mSource;
    
    size_t mSourceLength;
    size_t mLine = 1;
    size_t mColumn = 1;

    void skip_whitespace();
    std::optional<std::tuple<std::string, TokenKind>> match_literal();
    bool is_inside(size_t offset = 0);
    int utf8_width();
    void advance(size_t by = 1) {
        mCursor += by;
        mColumn++;
    }
    void move_heads(size_t by) {
        mCursor += by;
        mColumn += by;
    }

    friend LexerError;
};

class LexerError : public std::exception {
public:
    LexerError(Lexer *lexer, std::string message) : mMessage(message), mFilename(lexer->mFilename), mLine(lexer->mLine), mColumn(lexer->mColumn) {};

    const char *what() const noexcept {
        static std::string formattedMessage;
        std::ostringstream oss;
        oss << "ERROR " << mFilename << ":" << mLine << ":" << mColumn << ": " << mMessage;
        formattedMessage = oss.str();
        return formattedMessage.c_str();
    }

private:
    std::string mMessage;
    std::string mFilename;
    size_t mLine;
    size_t mColumn;
};
