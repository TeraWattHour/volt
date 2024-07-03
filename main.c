#include "lexer.h"
#include "parser.h"

int main(void) {
    Lexer lexer = lexer_init("test.volt");
    Parser parser = parser_init(&lexer);

    Expr *expr = parser_parse_expr(&parser);

    parser_free(&parser);
    return 0;
}
