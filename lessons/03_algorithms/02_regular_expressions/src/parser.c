#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	const char *regex;
	size_t index;
} Parser;

static ASTNode *parse_alter(Parser *);
static ASTNode *parse_concat(Parser *);
static ASTNode *parse_quantifier(Parser *);
static ASTNode *parse_range(Parser *, ASTNode *);
static ASTNode *parse_primary(Parser *);
static ASTNode *parse_anchor(Parser *);
static ASTNode *parse_class(Parser *);
static ASTNode *parse_group(Parser *);
static ASTNode *parse_char(Parser *);
static ASTNode *parse_escape(Parser *);

static void advance(Parser *);
static char peek(Parser *);
static int match(Parser *, char);
static void expect(Parser *, char);

static const char specials[] = "|?*+{}()[]^$.";

// Implementation

ASTNode *parse(const char *regex) {
	Parser parser = {regex, 0};
	return parse_alter(&parser);
}

static ASTNode *parse_alter(Parser *parser) {
	ASTNode *left = parse_concat(parser);
	while (match(parser, '|')) {
		ASTNode *right = parse_concat(parser);
		left = create_binary(NODE_ALTER, left, right);
	}
	return left;
}

static ASTNode *parse_concat(Parser *parser) {
	ASTNode *left = parse_quantifier(parser);
	while (peek(parser) && peek(parser) != '|' && peek(parser) != ')') {
		ASTNode *right = parse_quantifier(parser);
		left = create_binary(NODE_CONCAT, left, right);
	}
	return left;
}

static ASTNode *parse_quantifier(Parser *parser) {
	ASTNode *node = parse_primary(parser);

	if (match(parser, '?')) {
		node = create_quantifier(node, 0, 1);
	} else if (match(parser, '*')) {
		node = create_quantifier(node, 0, -1);
	} else if (match(parser, '+')) {
		node = create_quantifier(node, 1, -1);
	} else if (match(parser, '{')) {
		node = parse_range(parser, node);
	}

	return node;
}

static ASTNode *parse_range(Parser *parser, ASTNode *node) {
	int min = 0, max = -1;

	if (peek(parser) == '}') {
		fprintf(stderr, "Expected number\n");
		exit(1);
	}

	while (isdigit(peek(parser))) {
		min = min * 10 + (peek(parser) - '0');
		advance(parser);
	}

	if (match(parser, '}')) {
		return create_quantifier(node, min, min);
	}

	if (match(parser, ',')) {
		if (peek(parser) == '}') {
			max = -1;
		} else if (isdigit(peek(parser))) {
			max = 0;
			while (isdigit(peek(parser))) {
				max = max * 10 + (peek(parser) - '0');
				advance(parser);
			}
		} else {
			fprintf(stderr, "Expected number\n");
			exit(1);
		}
	}

	expect(parser, '}');
	return create_quantifier(node, min, max);
}

static ASTNode *parse_primary(Parser *parser) {
	if (match(parser, '^')) {
		return create_anchor(ANCHOR_START);
	} else if (match(parser, '$')) {
		return create_anchor(ANCHOR_END);
	} else if (match(parser, '.')) {
		return create_char_class(0, CLASS_DOT);
	} else if (match(parser, '[')) {
		return parse_class(parser);
	} else if (match(parser, '(')) {
		return parse_group(parser);
	} else {
		return parse_char(parser);
	}
}

#define DEFAULT_RANGE_SIZE 8

static ASTNode *parse_class(Parser *parser) {
	bool negated = 0;
	if (match(parser, '^')) {
		negated = 1;
	}

	size_t size = DEFAULT_RANGE_SIZE, cnt = 0;
	Range *ranges = malloc(size * sizeof(Range));

	while (peek(parser) && peek(parser) != ']') {
		if (cnt == size) {
			size *= 2;
			ranges = realloc(ranges, size * sizeof(Range));
		}

		char start = peek(parser);

		if (match(parser, '\\')) {
			static const char *specials = "[]^-\\";
			if (!strchr(specials, peek(parser))) {
				fprintf(stderr, "Unexpected character '\\'\n");
				exit(1);
			}
			start = peek(parser);
		}

		advance(parser);

		if (match(parser, '-')) {
			if (peek(parser) == ']') {
				fprintf(stderr, "Unexpected character '-'\n");
				exit(1);
			}

			char end = peek(parser);
			advance(parser);

			ranges[cnt++] = (Range){start, end};
		} else {
			ranges[cnt++] = (Range){start, start};
		}
	}

}

static ASTNode *parse_group(Parser *parser) {
	ASTNode *group = parse_alter(parser);
	expect(parser, ')');
	return create_group(group);
}

static ASTNode *parse_char(Parser *parser) {
	char c = peek(parser);
	
	if (strchr(specials, c)) {
		fprintf(stderr, "Unexpected character '%c'\n", c);
		exit(1);
	}

	if (match(parser, '\\')) {
		return parse_escape(parser);
	}

	advance(parser);
	return create_char(c);
}

static ASTNode *parse_escape(Parser *parser) {
	char c = peek(parser);
	advance(parser);
	switch (c) {
		case 'd': return create_char_class(0, CLASS_DIGIT);
		case 'D': return create_char_class(1, CLASS_DIGIT);
		case 'w': return create_char_class(0, CLASS_WORD);
		case 'W': return create_char_class(1, CLASS_WORD);
		case 's': return create_char_class(0, CLASS_SPACE);
		case 'S': return create_char_class(1, CLASS_SPACE);
		case 'b': return create_anchor(ANCHOR_WORD);
		case 'B': return create_anchor(ANCHOR_NON_WORD);
		default: break;
	}

	if (!strchr(specials, c)) {
		fprintf(stderr, "Unknown escape sequence '\\%c'\n", c);
		exit(1);
	}
}

// Helper functions

static void advance(Parser *parser) {
	parser->index++;
}

static char peek(Parser *parser) {
	return parser->regex[parser->index];
}

static int match(Parser *parser, char c) {
	if (peek(parser) == c) {
		advance(parser);
		return 1;
	}
	return 0;
}

static void expect(Parser *parser, char c) {
	if (!match(parser, c)) {
		fprintf(stderr, "Expected '%c'\n", c);
		exit(1);
	}
}