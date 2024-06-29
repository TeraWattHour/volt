#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "lexer.h"
#include "lexer_macros.h"

void lexer_parse_binary(Lexer *lexer, Token *token) {
    token->radix = 2;
    lexer->c += 2, lexer->column += 2;

    lexer_scan_digits(lexer, 2);
    token->end = lexer->c;
}

void lexer_parse_octal(Lexer *lexer, Token *token) {
    token->radix = 8;
    lexer->c += 2, lexer->column += 2;

    lexer_scan_digits(lexer, 8);
    token->end = lexer->c;
}

void lexer_parse_decimal(Lexer *lexer, Token *token) {
    token->radix = 10;

    lexer_scan_digits(lexer, 10);

    if (*lexer->c == '.') {
        advance();
        lexer_scan_digits(lexer, 10);

        if (tolower(*lexer->c) == 'e') {
            advance();

            if (*lexer->c == '+' || *lexer->c == '-' || isdigit(*lexer->c)) {
                advance();
            } else {
                ERROR("Unexpected character `%c`, decimal exponent must start with `+`, `-` or a decimal digit\n", *lexer->c);
            }

            lexer_scan_digits(lexer, 10);
        }
    }

    token->end = lexer->c;
}

void lexer_parse_hex(Lexer *lexer, Token *token) {
    assert(0 && "Unimplemented");
}