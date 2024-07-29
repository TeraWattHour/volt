#pragma once

#include "lexer/lexer.h"
#include "parser.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef INIT_POOL_SZ
#define INIT_POOL_SZ 128
#endif

#define consume() ({ Token consumed = parser->current; parser->current = parser->next; parser->next = lexer_next(parser->lexer); consumed; })
#define match(...) ({ bool matched = false; TokenKind to_match[] = {__VA_ARGS__}; for (size_t i = 0; i < sizeof(to_match)/sizeof(to_match[0]); ++i) if(to_match[i] == parser->current.kind) { matched = true; break; }; matched; })
#define pool_alloc(kind) ({                                                                     \
    if ((double) parser->kind##_pool_size >= (double) parser->kind##_pool_capacity * 0.75) {    \
        parser->kind##_pool_capacity *= 2;                                                      \
        void *temp = realloc(parser->kind##_pool, sizeof(Expr) * parser->kind##_pool_capacity); \
        assert(temp != NULL && "realloc failed");                                               \
        parser->kind##_pool = temp;                                                             \
    };                                                                                          \
    &parser->kind##_pool[parser->kind##_pool_size++];                                           \
})
#define ERROR(...)                                                                                                  \
    do {                                                                                                            \
        fprintf(stderr, "ERROR %s:%zu:%zu: ", parser->lexer->filename, parser->lexer->line, parser->lexer->column); \
        fprintf(stderr, __VA_ARGS__);                                                                               \
        fprintf(stderr, "\n");                                                                                      \
        exit(1);                                                                                                    \
    } while (0)