#include "expression.h"

#include "token.h"
#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

// Parsing related functions

Token *tokenize(const char*);
ASTNode *parse(const Token*);

// Implementation of the expression.h functions

ASTNode *create_expression(const char *input) {
	Token *tokens = tokenize(input);
	ASTNode *root = parse(tokens);
	for (size_t i = 0; tokens[i].type != TOKEN_END; ++i) {
		free(tokens[i].value);
	}
	free(tokens);
	return root;
}

void delete_expression(ASTNode *root) {
	delete_ast(root);
}

// Print the expression in infix notation

static void print_expression_helper(ASTNode *);

void print_expression(ASTNode *root) {
	print_expression_helper(root);
	printf("\n");
}

static void print_expression_helper(ASTNode *root) {
	if (root == NULL) return;
	switch (root->type) {
		case NODE_BINARY:
			print_expression_helper(root->binary.left);
			printf("%s", root->binary.op);
			print_expression_helper(root->binary.right);
			break;
		case NODE_UNARY:
			printf("%s", root->unary.op);
			print_expression_helper(root->unary.base);
			break;
		case NODE_FUNC:
			printf("%s(", root->func.name);
			for (size_t i = 0; i < root->func.argc; ++i) {
				print_expression_helper(root->func.args[i]);
				if (i < root->func.argc - 1) {
					printf(", ");
				}
			}
			printf(")");
			break;
		case NODE_GROUP:
			printf("(");
			print_expression_helper(root->group);
			printf(")");
			break;
		case NODE_VAR:
			printf("%s", root->var);
			break;
		default: // NODE_NUM
			printf("%g", root->num);
			break;
	}
}