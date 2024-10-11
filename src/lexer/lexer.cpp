#include <iostream>
#include <regex>
#include <string>
#include <map>
#include <unordered_map>
#include <fmt/core.h>

#include <assert.h>
#include <ctype.h>
#include <regex.h>
#include <stdbool.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

#define TOKEN(name) \
    return (Token) { .kind = TokenKind::name, .start = mCursor, .end = ++mCursor, .line = mLine, .column = mColumn++ }
#define TOKEN2(name, a)                                                                                                             \
    do                                                                                                                              \
        if (is_inside(1) && *(mCursor + 1) == a) {                                                                                  \
            Token t2 = (Token){.kind = TokenKind::name, .start = mCursor, .end = mCursor += 2, .line = mLine, .column = mColumn++}; \
            mColumn++;                                                                                                              \
            return t2;                                                                                                              \
        }                                                                                                                           \
    while (0)
#define COMPARE_DIGRAPH(a) (is_inside(1) && *mCursor == a[0] && *(mCursor + 1) == a[1])

int _utf8_width(char c);
bool is_digit(char c, int radix);
bool is_spacer(char c);
bool is_huggable(char c);

static const std::regex decimal_regex("^(0|([1-9]+(_\\d+)*))(.(\\d+(_\\d+)*?))?(e(\\+|-)?[1-9]+(_\\d+)*)?");
static const std::regex binary_regex("^0b[01]+(_[01]+)*");
static const std::regex octal_regex("^0o[0-7]+(_[0-7]+)*");
static const std::regex hex_regex("^0x[0-9a-fA-F]+(_[0-9a-fA-F]+)*");

void remove_underscores(std::string &str) {
    str.erase(std::remove(str.begin(), str.end(), '_'), str.end());
}

namespace tokens {
using enum TokenKind;

std::optional<std::tuple<std::string, TokenKind>> match_literal(char *cur) {
    auto found = std::find_if(operators.begin(), operators.end(), [&cur](auto &kv) {
        auto literal = std::get<0>(kv);
        return strncmp(cur, literal.c_str(), literal.length()) == 0;
    });
    
    if (found == operators.end()) return {};
    return *found;
}

std::string literal_for_operator(TokenKind op) {
    auto found = std::find_if(operators.begin(), operators.end(), [&op](auto &kv) {
        return std::get<1>(kv) == op;
    });
    assert(found != operators.end());
    return std::get<0>(*found);
    return "";
}
}


Token Lexer::next() {
    skip_whitespace();
    
    Token token = {
        .start = mCursor,
        .end = mCursor,
        .line = mLine,
        .column = mColumn
    };

    if (!is_inside()) TOKEN(_EOF);
    
    switch (*mCursor) {
        case '*': TOKEN(STAR);
        case '/': TOKEN(SLASH);
        case '%': TOKEN(PERCENT);
        case '+': TOKEN2(INCREMENT, '+'); TOKEN(PLUS);
        case '-': TOKEN2(DECREMENT, '-'); TOKEN(MINUS);
        case '&': TOKEN2(AND, '&'); TOKEN(AMPERSAND);
        case '|': TOKEN2(OR, '|'); TOKEN(PIPE);
        case '(': TOKEN(LPAREN);
        case '{': TOKEN(LBRACE);
        case '}': TOKEN(RBRACE);
        case ')': TOKEN(RPAREN);
        case '[': TOKEN(LBRACKET);
        case ']': TOKEN(RBRACKET);
        case '=': TOKEN2(EQ, '='); TOKEN(ASSIGN);
        case '!': TOKEN2(NEQ, '='); TOKEN(BANG);
        case '>': TOKEN2(GTE, '='); TOKEN(GT);
        case '<': TOKEN2(LTE, '='); TOKEN(LT);
        case ':': TOKEN2(COLON_COLON, ':'); TOKEN(COLON);
        case ';': TOKEN(SEMICOLON);
        case '.': if (!is_inside(1) || !isnumber(*(mCursor + 1))) TOKEN(DOT);
        case '\'': {
            token.kind = TokenKind::RUNE;
            advance();

            advance(utf8_width());

            if (*mCursor != '\'') {
                throw LexerError(this, "unclosed rune");
            }

            advance();

            token.end = mCursor;

            return token;
        }
        case '"': {
            token.kind = TokenKind::STRING;
            advance();

            while (*mCursor && *mCursor != '"') {
                advance(utf8_width());
            }

            if (*mCursor != '"') {
                throw LexerError(this, "unclosed string literal");
            }

            advance();

            token.end = mCursor;

            return token;
        }
        default: {
            char *start = mCursor;
            
            if (isalpha(*mCursor) || *mCursor == '_') {
                while (is_inside() && (isalnum(*mCursor) || *mCursor == '_')) {
                    advance();
                }
                
                if (utf8_width() != 1) {
                    throw LexerError(this, "illegal non-ASCII character");
                }
                
                auto keyword = tokens::operators.find(std::string(start, mCursor - start));
            
                token.kind = keyword != tokens::operators.end() ? (*keyword).second : TokenKind::IDENTIFIER;
                token.end = mCursor;

                return token;
            } else if (*mCursor == '.' || isnumber(*mCursor)) {
                token.kind = TokenKind::NUMBER;

                std::smatch match;
                std::string str(mCursor);
                if (std::regex_search(str, match, binary_regex) || std::regex_search(str, match, octal_regex) || std::regex_search(str, match, hex_regex) || std::regex_search(str, match, decimal_regex)) {
                    move_heads(match.length());
                } else {
                    throw LexerError(this, "unrecognized numeric literal");
                }

                if (!is_huggable(*mCursor)) {
                    throw LexerError(this, "unexpected character in numeric literal");
                }

                token.end = mCursor;

                return token;
            }

            throw LexerError(this, "unexpected character");
        }
    }
}

void Lexer::skip_whitespace() {
    size_t block_depth = 0;
    while (is_inside() && (block_depth > 0 || isspace(*mCursor) || COMPARE_DIGRAPH("//") || COMPARE_DIGRAPH("/*"))) {
        if (COMPARE_DIGRAPH("//")) {
            while (is_inside() && *mCursor != '\n') mCursor++;
        } else if (COMPARE_DIGRAPH("/*")) {
            block_depth++; advance();
        } else if (COMPARE_DIGRAPH("*/")) {
            block_depth--; advance();
        }

        if (*mCursor == '\n') { mLine++; mColumn = 1; }
        else mColumn++;

        mCursor++;
    }
}

bool Lexer::is_inside(size_t offset) {
    return mCursor != nullptr && *mCursor && mCursor - mSource + offset < mSourceLength;
}

bool is_digit(char c, int radix) {
    if (radix == 10) return isdigit(c);
    else if (radix == 16) return isxdigit(c);
    else if (radix == 8) return c >= '0' && c <= '7';
    else if (radix == 2) return c == '0' || c == '1';
    return false;
}

bool is_spacer(char c) { return c == '_' || c == '\''; }
bool is_huggable(char c) { return c != '_' && !isalnum(c) && _utf8_width(c) == 1; }

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << token.kind;
    return os;
}

std::ostream &operator<<(std::ostream &os, const TokenKind &kind) {
#define CASE(name) \
    case TokenKind::name: os << tokens::literal_for_operator(TokenKind::name); break;
    
    switch (kind) {
        CASE(_EOF)
        CASE(IDENTIFIER)
        CASE(NUMBER)
        CASE(STRING)
        CASE(RUNE)
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
        CASE(LET)
        CASE(MUT)
        CASE(STRUCT)
        CASE(BREAK)
        CASE(CONTINUE)
        CASE(RETURN)
        CASE(MATCH)

        CASE(SEMICOLON)
        CASE(COLON)
        CASE(COLON_COLON)

        CASE(ASSIGN)
        CASE(GT)
        CASE(LT)
        CASE(LTE)
        CASE(GTE)
        CASE(NEQ)
        CASE(EQ)
        CASE(BANG)
        CASE(DOT)
        CASE(PLUS)
        CASE(INCREMENT)
        CASE(DECREMENT)
        CASE(MINUS)
        CASE(STAR)
        CASE(SLASH)
        CASE(PERCENT)
        CASE(AMPERSAND)
        CASE(PIPE)
        CASE(AND)
        CASE(OR)
        CASE(LBRACKET)
        CASE(RBRACKET)
        CASE(QUESTION)
    }

    return os;
}

int Lexer::utf8_width() {
    if (!is_inside()) return 1;
        
    int width = _utf8_width(*mCursor);
    if (width == 0) throw LexerError(this, "invalid utf-8 character");
    return width;
}

int _utf8_width(char c) {
    if ((c & 0x80) == 0) return 1;
    else if ((c & 0xE0) == 0xC0) return 2;
    else if ((c & 0xF0) == 0xE0) return 3;
    else if ((c & 0xF8) == 0xF0) return 4;
    return 0;
}
