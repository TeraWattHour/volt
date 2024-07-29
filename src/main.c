#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/printer.h"

int main(void) {
    Lexer lexer = lexer_init("test.volt");
    Parser parser = parser_init(&lexer);

    parser_let(&parser);
    parser_free(&parser);
    return 0;
}


