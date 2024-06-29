#ifndef VOLT_LEXER_H
#define VOLT_LEXER_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    const char *filename;

    // source is a null terminated string of source_len length (excluding the null terminator)
    const char *source;
    const size_t source_len;

    char *c;

    size_t line;
    size_t column;
} Lexer;

typedef enum {
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NIL,
    TOKEN_IF,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_FOR,
    TOKEN_CONST,
    TOKEN_LET,
    TOKEN_STRUCT,
    TOKEN_BREAK,
    TOKEN_CONTINUE,
    TOKEN_RETURN,
    TOKEN_MATCH,
    TOKEN_SEMICOLON,
    TOKEN_ASSIGN,
    TOKEN_EQUALS,
    TOKEN_GT,
    TOKEN_LT,
    TOKEN_LTE,
    TOKEN_GTE,
    TOKEN_NEQ,
    TOKEN_EQ,
    TOKEN_BANG,
    TOKEN_DOT,
} TokenKind;

typedef struct {
    TokenKind kind;
    char *start;
    char *end;
    size_t line;
    size_t column;
    union {
        int radix;
    };
} Token;

typedef struct {
    const TokenKind kind;
    const char *keyword;
} KeywordMapping;

Lexer lexer_init(const char *filename);
Token lexer_next(Lexer *lexer);
int lexer_free(Lexer *lexer);

void lexer_parse_decimal(Lexer *lexer, Token *token);
void lexer_parse_hex(Lexer *lexer, Token *token);
void lexer_parse_octal(Lexer *lexer, Token *token);
void lexer_parse_binary(Lexer *lexer, Token *token);

void lexer_skip_whitespace(Lexer *lexer);
void lexer_scan_digits(Lexer *lexer, int base);

bool is_digit(char c, int radix);
bool is_spacer(char c);
bool is_huggable(char c);

TokenKind identifier_kind(const unsigned char *start, const unsigned char *end);

char *token_kind_to_name(TokenKind kind);

#endif //VOLT_LEXER_H
