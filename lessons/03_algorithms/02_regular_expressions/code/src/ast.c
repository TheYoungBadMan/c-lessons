#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

ASTNode *create_node(NodeType type) {
	ASTNode *node = malloc(sizeof(ASTNode));
	node->type = type;
	return node;
}

ASTNode *create_binary(NodeType type, ASTNode *left, ASTNode *right) {
	ASTNode *node = create_node(type);
	node->binary.left = left;
	node->binary.right = right;
	return node;
}

ASTNode *create_quantifier(ASTNode *base, int min, int max) {
	ASTNode *node = create_node(NODE_QUANTIFIER);
	node->quantifier.base = base;
	node->quantifier.min = min;
	node->quantifier.max = max;
	return node;
}

ASTNode *create_anchor(AnchorType type) {
	ASTNode *node = create_node(NODE_ANCHOR);
	node->anchor = type;
	return node;
}

ASTNode *create_char_class(bool negated, ClassType type) {
	ASTNode *node = create_node(NODE_CHAR_CLASS);
	node->char_class.negated = negated;
	node->char_class.type = type;
	return node;
}

ASTNode *create_group(ASTNode *group) {
	ASTNode *node = create_node(NODE_GROUP);
	node->group = group;
	return node;
}

ASTNode *create_char(char value) {
	ASTNode *node = create_node(NODE_CHAR);
	node->value = value;
	return node;
}

void delete_ast(ASTNode *root) {
	if (root == NULL) {
		return;
	}

	switch (root->type) {
		case NODE_ALTER:
		case NODE_CONCAT:
			delete_ast(root->binary.left);
			delete_ast(root->binary.right);
			break;

		case NODE_QUANTIFIER:
			delete_ast(root->quantifier.base);
			break;

		case NODE_GROUP:
			delete_ast(root->group);
			break;

		default:
			break;
	}

	free(root);
}