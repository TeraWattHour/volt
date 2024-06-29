#include <stdio.h>
#include "lexer.h"

int main(void) {
    Lexer lexer = lexer_init("test.volt");

    while (1) {
        Token token = lexer_next(&lexer);

        printf("Token: %s %.*s %zu:%zu\n", token_kind_to_name(token.kind), (int)(token.end - token.start), token.start, token.line, token.column);
        if (token.kind == TOKEN_EOF) {
            break;
        }

        if (token.kind == TOKEN_NUMBER && token.radix == 16) {
            printf("Hexadecimal number, core: %.*s, mantissa: %.*s, exponent: %.*s \n", (int)(token.hex_literal.mantissa_start - 1 - token.start - 2), token.start + 2, (int)(token.hex_literal.exponent_start - 1 - token.hex_literal.mantissa_start), token.hex_literal.mantissa_start, (int)(token.end - token.hex_literal.exponent_start), token.hex_literal.exponent_start);
        }
    }

    lexer_free(&lexer);
    return 0;
}
