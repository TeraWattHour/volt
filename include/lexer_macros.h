#ifndef VOLT_LEXER_MACROS_H
#define VOLT_LEXER_MACROS_H

#define advance() lexer->c++, lexer->column++
#define ERROR(message, ...) fprintf(stderr, "ERROR %s:%zu:%zu: " message, lexer->filename, lexer->line, lexer->column, __VA_ARGS__), exit(1)
#define TOKEN(name) return (Token) { .kind = TOKEN_##name, .start = lexer->c, .end = lexer->c++, .line = lexer->line, .column = lexer->column++ }
#define TOKEN2(name, a)                                                                                                \
    do                                                                                                                 \
        if (*(lexer->c + 1) == a) {                                                                                    \
            Token t2 = (Token) {.kind = TOKEN_##name, .start = lexer->c, .end = lexer->c += 2, .line = lexer->line, .column = lexer->column++ }; \
            lexer->column++;                                                                                           \
            return t2;                                                                                                 \
        }                                                                                                              \
    while (0)
#define COMPARE_DIGRAPH(a) (*lexer->c == a[0] && *(lexer->c + 1) == a[1])


#endif //VOLT_LEXER_MACROS_H
