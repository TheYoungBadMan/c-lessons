#pragma once

#include "ast.h"

#include <stdbool.h>

ASTNode *create_regex(const char *);
void delete_regex(ASTNode *);

bool regex_match(ASTNode *, const char *);
