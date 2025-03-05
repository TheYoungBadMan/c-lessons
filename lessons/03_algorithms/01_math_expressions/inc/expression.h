#pragma once

#include "ast.h"

typedef struct {
	const char *name;
	double value;
} Variable;

typedef struct {
	const char *name;
	double (*func)(double*, size_t);
} Function;

ASTNode *create_expression(const char *);
void delete_expression(ASTNode *);

void print_expression(ASTNode *);
double evaluate_expression(ASTNode *, Variable *, size_t, Function *, size_t);