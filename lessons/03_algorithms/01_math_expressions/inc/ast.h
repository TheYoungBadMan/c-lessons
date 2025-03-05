#pragma once

#include <stddef.h>

typedef enum {
	NODE_BINARY,
	NODE_UNARY,
	NODE_FUNC,
	NODE_GROUP,
	NODE_VAR,
	NODE_NUM,
} NodeType;

typedef struct ASTNode {
	NodeType type;

	union {
		struct {
			char *op;
			struct ASTNode* left;
			struct ASTNode* right;
		} binary; // NODE_BINARY

		struct {
			char *op;
			struct ASTNode* base;
		} unary; // NODE_UNARY

		struct {
			char* name;
			size_t argc;
			struct ASTNode** args;
		} func; // NODE_FUNC

		struct ASTNode* group; // NODE_GROUP

		char *var; // NODE_VAR

		double num; // NODE_NUM
	};

} ASTNode;

ASTNode* create_node(NodeType);
ASTNode* create_binary(const char *, ASTNode*, ASTNode*);
ASTNode* create_unary(const char *, ASTNode*);
ASTNode* create_func(const char*, size_t, ASTNode**);
ASTNode* create_group(ASTNode*);
ASTNode* create_var(const char*);
ASTNode* create_number(const char*);

void delete_ast(ASTNode*);