#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "lexer.h"
#include "lexer_macros.h"

void lexer_scan_binary(Lexer *lexer, Token *token) {
    token->numeric_literal.radix = 2;
    lexer->c += 2, lexer->column += 2;

    lexer_scan_digits(lexer, 2);
    token->end = lexer->c;
}

void lexer_scan_octal(Lexer *lexer, Token *token) {
    token->numeric_literal.radix = 8;
    lexer->c += 2, lexer->column += 2;

    lexer_scan_digits(lexer, 8);
    token->end = lexer->c;
}

void lexer_scan_decimal(Lexer *lexer, Token *token) {
    token->numeric_literal.radix = 10;

    lexer_scan_digits(lexer, 10);

    if (*lexer->c == '.') {
        advance();
        token->numeric_literal.is_float = true;
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

void lexer_scan_hex(Lexer *lexer, Token *token) {
    token->numeric_literal.radix = 16;
    lexer->c += 2, lexer->column += 2;

    lexer_scan_digits(lexer, 16);

    if (*lexer->c == '.') {
        advance();
        token->numeric_literal.is_float = true;
//        token->hex_literal.mantissa_start = lexer->c;
        lexer_scan_digits(lexer, 16);

        if (tolower(*lexer->c) == 'p') {
            advance();
//            token->hex_literal.exponent_start = lexer->c;
            if (*lexer->c == '+' || *lexer->c == '-' || isdigit(*lexer->c)) {
                advance();
            } else {
                ERROR("Unexpected character `%c`, hex exponent must start with `+`, `-` or a decimal digit\n", *lexer->c);
            }

            lexer_scan_digits(lexer, 10);
        }
    }

    token->end = lexer->c;
}