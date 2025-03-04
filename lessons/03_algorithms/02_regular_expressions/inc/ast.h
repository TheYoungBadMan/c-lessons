#pragma once

#include <stdbool.h>

typedef enum {
	NODE_ALTER, // |
	NODE_CONCAT,
	NODE_QUANTIFIER, // ? * + { }
	NODE_ANCHOR, // ^ $
	NODE_CHAR_CLASS, // . \d \D \w \W \s \S
	NODE_CUSTOM_CLASS, // []
	NODE_GROUP, // ( )
	NODE_CHAR, // any character
} NodeType;

typedef enum {
	ANCHOR_START, // ^
	ANCHOR_END, // $
	ANCHOR_WORD, // \b
	ANCHOR_NON_WORD, // \B
} AnchorType;

typedef enum {
	CLASS_DOT,
	CLASS_DIGIT,
	CLASS_WORD,
	CLASS_SPACE,
} ClassType;

typedef struct {
	char start, end;
} Range;

typedef struct ASTNode {

	NodeType type;

	union {
		struct {
			struct ASTNode* left;
			struct ASTNode* right;
		} binary; // NODE_ALTER, NODE_CONCAT

		struct {
			struct ASTNode* base;
			int min, max;
		} quantifier; // NODE_QUANTIFIER
		

		AnchorType anchor; // NODE_ANCHOR

		struct {
			bool negated;
			ClassType type;
		} char_class;

		struct {
			bool negated;
			Range *ranges;
			size_t cnt;
		} custom_class; // NODE_CUSTOM_CLASS


		struct ASTNode* group; // NODE_GROUP

		char value; // NODE_CHAR

	};

} ASTNode;

ASTNode *create_node(NodeType);
ASTNode *create_binary(NodeType, ASTNode*, ASTNode*);
ASTNode *create_quantifier(ASTNode*, int, int);
ASTNode *create_anchor(AnchorType);
ASTNode *create_char_class(bool, ClassType);
ASTNode *create_custom_class(bool, Range*, size_t);
ASTNode *create_group(ASTNode*);
ASTNode *create_char(char);

void delete_ast(ASTNode*);