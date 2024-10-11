#pragma once

#include "parser.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define match(...) ({ bool matched = false; TokenKind to_match[] = {__VA_ARGS__}; for (size_t i = 0; i < sizeof(to_match)/sizeof(to_match[0]); ++i) if(to_match[i] == this->current.kind) { matched = true; break; }; matched; })
