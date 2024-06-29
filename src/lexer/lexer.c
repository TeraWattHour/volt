#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

#include "lexer.h"
#include "utf8.h"
#include "lexer_macros.h"

static const KeywordMapping keyword_mappings[] = {
    {TOKEN_CONST, "const"},
    {TOKEN_LET, "let"},
    {TOKEN_TRUE, "true"},
    {TOKEN_FALSE, "false"},
    {TOKEN_NIL, "nil"},
    {TOKEN_IF, "if"},
    {TOKEN_ELIF, "elif"},
    {TOKEN_ELSE, "else"},
    {TOKEN_FOR, "for"},
    {TOKEN_STRUCT, "struct"},
    {TOKEN_BREAK, "break"},
    {TOKEN_CONTINUE, "continue"},
    {TOKEN_RETURN, "return"},
    {TOKEN_MATCH, "match"},
};

Lexer lexer_init(const char *filename) {
    assert(filename != NULL);

    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "ERROR %s: Could not open file\n", filename);
        exit(1);
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "ERROR %s: Could not seek to end of file\n", filename);
        exit(1);
    }

    size_t source_len = ftell(file);

    if (fseek(file, 0, SEEK_SET) != 0) {
        fprintf(stderr, "ERROR %s: Could not seek to start of file\n", filename);
        exit(1);
    }

    char *source = malloc(source_len + 1);

    size_t chars_read = fread(source, 1, source_len, file);
    if (chars_read != source_len) {
        fprintf(stderr, "ERROR %s: Could not read file\n", filename);
        exit(1);
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "ERROR %s: Could not close file\n", filename);
        exit(1);
    }

    return (Lexer) {
            .filename = filename,
            .source_len = source_len,
            .source = source,
            .c = source,
            .line = 1,
            .column = 1
    };
}

Token lexer_next(Lexer *lexer) {
    lexer_skip_whitespace(lexer);

    Token token = {
        .start = lexer->c,
        .end = lexer->c,
        .line = lexer->line,
        .column = lexer->column
    };

    switch (*lexer->c) {
        case '\0': TOKEN(EOF);
        case '(': TOKEN(LPAREN);
        case '{': TOKEN(LBRACE);
        case '}': TOKEN(RBRACE);
        case ')': TOKEN(RPAREN);
        case '=': TOKEN2(EQUALS, '='); TOKEN(ASSIGN);
        case '!': TOKEN2(NEQ, '='); TOKEN(BANG);
        case '>': TOKEN2(GTE, '='); TOKEN(GT);
        case '<': TOKEN2(LTE, '='); TOKEN(LT);
        case ';': TOKEN(SEMICOLON);
        case '.': if (!isnumber(*(lexer->c + 1))) TOKEN(DOT);
        default: {
            unsigned char width = utf8_width(*lexer->c);

            if (isalpha(*lexer->c) || *lexer->c == '_' || width != 1) {
                bool can_be_keyword = isalpha(*lexer->c);

                while (*lexer->c && (width = utf8_width(*lexer->c)) != 1 || isalnum(*lexer->c) || *lexer->c == '_' ) {
                    can_be_keyword &= isalpha(*lexer->c);
                    lexer->c += width;
                    lexer->column++;
                }

                token.kind = can_be_keyword ? identifier_kind((unsigned char *)token.start, (unsigned char *)lexer->c) : TOKEN_IDENTIFIER;
                token.end = lexer->c;

                return token;
            } else if (isnumber(*lexer->c) || *lexer->c == '.') {
                token.kind = TOKEN_NUMBER;

                if (COMPARE_DIGRAPH("0x") || COMPARE_DIGRAPH("0X")) {
                    token.radix = 16;
                    lexer->c += 2, lexer->column += 2;
                } else if (COMPARE_DIGRAPH("0b") || COMPARE_DIGRAPH("0B")) {
                    lexer_parse_binary(lexer, &token);
                } else if (COMPARE_DIGRAPH("0o") || COMPARE_DIGRAPH("0O")) {
                    lexer_parse_octal(lexer, &token);
                } else {
                    if (*lexer->c == '0' && isdigit(*(lexer->c + 1))) {
                        fprintf(stderr, "ERROR %s:%zu:%zu: Unexpected character '%c'\n", lexer->filename, lexer->line, lexer->column, *lexer->c);
                        exit(1);
                    }

                    lexer_parse_decimal(lexer, &token);
                }

                if (!is_huggable(*lexer->c)) {
                    fprintf(stderr, "ERROR %s:%zu:%zu: Unexpected character %c\n", lexer->filename, lexer->line, lexer->column, *lexer->c);
                    exit(1);
                }

                return token;
            }
        }
    }

    fprintf(stderr, "ERROR %s:%zu:%zu: Unexpected character '%c'\n", lexer->filename, lexer->line, lexer->column, *lexer->c);
    exit(1);
}

void lexer_skip_whitespace(Lexer *lexer) {
    size_t block_depth = 0;
    while (*lexer->c && (block_depth > 0 || isspace(*lexer->c) || COMPARE_DIGRAPH("//") || COMPARE_DIGRAPH("/*"))) {
        if (COMPARE_DIGRAPH("//")) {
            while (*lexer->c && *lexer->c != '\n')
                lexer->c++;
        } else if (COMPARE_DIGRAPH("/*")) {
            block_depth++, advance();
        } else if (COMPARE_DIGRAPH("*/")) {
            block_depth--, advance();
        }

        if (*lexer->c == '\n') lexer->line++, lexer->column = 1;
        else lexer->column++;

        lexer->c++;
    }
}

void lexer_scan_digits(Lexer *lexer, int base) {
    bool can_place_spacer = false;

    while (is_digit(*lexer->c, base) || is_spacer(*lexer->c)) {
        if (is_spacer(*lexer->c) && !can_place_spacer) {
            fprintf(stderr, "ERROR %s:%zu:%zu: Unexpected character %c\n", lexer->filename, lexer->line, lexer->column, *lexer->c);
            exit(1);
        }
        can_place_spacer = !is_spacer(*lexer->c);
        advance();
    }
}

int lexer_free(Lexer *lexer) {
    free((void *)lexer->source);
    return 0;
}

TokenKind identifier_kind(const unsigned char *start, const unsigned char *end) {
    for (size_t i = 0; i < sizeof(keyword_mappings) / sizeof(keyword_mappings[0]); i++) {
        if (strncmp((const char *)start, keyword_mappings[i].keyword, (int)(end - start)) == 0) {
            return keyword_mappings[i].kind;
        }
    }

    return TOKEN_IDENTIFIER;
}

bool is_digit(char c, int radix) {
    if (radix == 10) {
        return isdigit(c);
    } else if (radix == 16) {
        return isxdigit(c);
    } else if (radix == 8) {
        return c >= '0' && c <= '7';
    } else if (radix == 2) {
        return c == '0' || c == '1';
    }

    return false;
}

bool is_spacer(char c) {
    return c == '_' || c == '\'';
}

bool is_huggable(char c) { return !isascii(c) || !isalnum(c) && c != '_'; }

char *token_kind_to_name(TokenKind kind) {
#define CASE(name) case TOKEN_##name: return #name;
    switch (kind) {
        CASE(EOF)
        CASE(IDENTIFIER)
        CASE(NUMBER)
        CASE(STRING)
        CASE(CHAR)
        CASE(LPAREN)
        CASE(RPAREN)
        CASE(LBRACE)
        CASE(RBRACE)
        CASE(TRUE)
        CASE(FALSE)
        CASE(NIL)
        CASE(IF)
        CASE(ELIF)
        CASE(ELSE)
        CASE(FOR)
        CASE(CONST)
        CASE(LET)
        CASE(STRUCT)
        CASE(BREAK)
        CASE(CONTINUE)
        CASE(RETURN)
        CASE(MATCH)
        CASE(SEMICOLON)
        CASE(ASSIGN)
        CASE(EQUALS)
        CASE(GT)
        CASE(LT)
        CASE(LTE)
        CASE(GTE)
        CASE(NEQ)
        CASE(EQ)
        CASE(BANG)
        CASE(DOT)
    }

    assert(false && "Unknown token kind");
#undef CASE
}
