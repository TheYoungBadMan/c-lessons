#include "ast.h"

#include <stdlib.h>
#include <string.h>

ASTNode* create_node(NodeType type) {
	ASTNode* node = (ASTNode*) malloc(sizeof(ASTNode));
	node->type = type;
	return node;
}

ASTNode* create_binary(const char *op, ASTNode *left, ASTNode *right) {
	ASTNode* node = create_node(NODE_BINARY);
	node->binary.op = strdup(op);
	node->binary.left = left;
	node->binary.right = right;
	return node;
}

ASTNode* create_unary(const char *op, ASTNode *base) {
	ASTNode* node = create_node(NODE_UNARY);
	node->unary.op = strdup(op);
	node->unary.base = base;
	return node;
}

ASTNode* create_func(const char *name, size_t argc, ASTNode **args) {
	ASTNode* node = create_node(NODE_FUNC);
	node->func.name = strdup(name);
	node->func.argc = argc;
	node->func.args = args;
	return node;
}

ASTNode* create_group(ASTNode *group) {
	ASTNode* node = create_node(NODE_GROUP);
	node->group = group;
	return node;
}

ASTNode* create_var(const char *name) {
	ASTNode* node = create_node(NODE_VAR);
	node->var = strdup(name);
	return node;
}

ASTNode* create_number(const char *value) {
	ASTNode* node = create_node(NODE_NUM);
	node->num = atof(value);
	return node;
}

void delete_ast(ASTNode *root) {
	if (root == NULL) return;
	switch (root->type) {
		case NODE_BINARY:
			delete_ast(root->binary.left);
			delete_ast(root->binary.right);
			free(root->binary.op);
			break;
		case NODE_UNARY:
			delete_ast(root->unary.base);
			free(root->unary.op);
			break;
		case NODE_FUNC:
			for (size_t i = 0; i < root->func.argc; ++i) {
				delete_ast(root->func.args[i]);
			}
			free(root->func.name);
			free(root->func.args);
			break;
		case NODE_GROUP:
			delete_ast(root->group);
			break;
		case NODE_VAR:
			free(root->var);
			break;
		case NODE_NUM:
			break;
	}
	free(root);
}