#pragma once

#include <stdlib.h>
#include <assert.h>
#include "lexer.h"
#include "parser.h"

void ast_print(Expr *expr, size_t indent);