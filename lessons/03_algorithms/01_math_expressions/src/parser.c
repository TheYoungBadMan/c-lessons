#include "token.h"
#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DEFAULT_ARGS 4

// Declaration of the parser

typedef struct {
	const Token *tokens;
	size_t index;
} Parser;

static ASTNode *parse_add(Parser *);
static ASTNode *parse_mul(Parser *);
static ASTNode *parse_pow(Parser *);
static ASTNode *parse_unary(Parser *);
static ASTNode *parse_primary(Parser *);
static ASTNode *parse_func(Parser *);
static ASTNode *parse_group(Parser *);

static void advance(Parser *);
static const Token *current(Parser *);
static const Token *previous(Parser *);
static bool match(Parser *, TokenType);
static void expect(Parser *, TokenType);

// Implementation of the parser

ASTNode *parse(const Token *tokens) {
	Parser parser = {tokens, 0};
	return parse_add(&parser);
}

static ASTNode *parse_add(Parser *parser) {
	ASTNode *left = parse_mul(parser);
	while (match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS)) {
		char *op = previous(parser)->value;
		ASTNode *right = parse_mul(parser);
		left = create_binary(op, left, right);
	}
	return left;
}

static ASTNode *parse_mul(Parser *parser) {
	ASTNode *left = parse_pow(parser);
	while (match(parser, TOKEN_STAR) || match(parser, TOKEN_SLASH)) {
		char *op = previous(parser)->value;
		ASTNode *right = parse_pow(parser);
		left = create_binary(op, left, right);
	}
	return left;
}

static ASTNode *parse_pow(Parser *parser) {
	ASTNode *left = parse_unary(parser);
	if (match(parser, TOKEN_CARET)) {
		ASTNode *right = parse_pow(parser);
		return create_binary("^", left, right);
	}
	return left;
}

static ASTNode *parse_unary(Parser *parser) {
	if (match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS)) {
		char *op = previous(parser)->value;
		ASTNode *base = parse_unary(parser);
		return create_unary(op, base);
	}
	return parse_primary(parser);
}

static ASTNode *parse_primary(Parser *parser) {
	if (match(parser, TOKEN_ID))
		return parse_func(parser);

	if (match(parser, TOKEN_LPAREN))
		return parse_group(parser);

	if (match(parser, TOKEN_NUM))
		return create_number(previous(parser)->value);

	fprintf(stderr, "Unexpected token: %s\n", current(parser)->value);
	exit(EXIT_FAILURE);
}

static ASTNode *parse_func(Parser *parser) {
	char *name = previous(parser)->value;

	if (match(parser, TOKEN_LPAREN)) {
		size_t capacity = DEFAULT_ARGS, i = 0;
		ASTNode **args = (ASTNode **) malloc(sizeof(ASTNode *) * capacity);

		if (!match(parser, TOKEN_RPAREN)) {
			do {
				if (i == capacity) {
					capacity += DEFAULT_ARGS;
					args = (ASTNode **) realloc(args, sizeof(ASTNode *) * capacity);
				}

				args[i++] = parse_add(parser);
			} while (match(parser, TOKEN_COMMA));
			expect(parser, TOKEN_RPAREN);
		}

		return create_func(name, i, args);
	}

	return create_var(name);
}

static ASTNode *parse_group(Parser *parser) {
	ASTNode *group = parse_add(parser);
	expect(parser, TOKEN_RPAREN);
	return group;
}

static void advance(Parser *parser) {
	++parser->index;
}

static const Token *current(Parser *parser) {
	return &parser->tokens[parser->index];
}

static const Token *previous(Parser *parser) {
	return &parser->tokens[parser->index - 1];
}

static bool match(Parser *parser, TokenType type) {
	if (current(parser)->type == type) {
		advance(parser);
		return true;
	}
	return false;
}

static void expect(Parser *parser, TokenType type) {
	if (!match(parser, type)) {
		fprintf(stderr, "Expected token: %d\n", type);
		exit(EXIT_FAILURE);
	}
}