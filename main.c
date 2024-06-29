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
    }

    lexer_free(&lexer);
    return 0;
}
