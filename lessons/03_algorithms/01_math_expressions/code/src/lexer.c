//lexer.c

#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define DEFAULT_CAPACITY 32

// Declaration of the lexer

typedef struct {
	const char *input;
	size_t index;
} Lexer;

static Token extract(Lexer *);
static Token extract_id(Lexer *);
static Token extract_num(Lexer *);
static Token extract_op(Lexer *);

static void advance(Lexer *, size_t);
static char peek(Lexer *);
static bool match(Lexer *, char);
static bool match_space(Lexer *);
static bool match_alpha(Lexer *);
static bool match_digit(Lexer *);
static bool match_alnum(Lexer *);
static bool match_end(Lexer *);

// Implementation of the lexer

Token *tokenize(const char *input) {
	Lexer lexer = {input, 0};
	size_t capacity = DEFAULT_CAPACITY;
	Token *tokens = (Token *) malloc(capacity * sizeof(Token));

	for (size_t i = 0; !match_end(&lexer); ++i) {
		if (i == capacity) {
			capacity += DEFAULT_CAPACITY;
			tokens = (Token *) realloc(tokens, capacity * sizeof(Token));
		}

		Token token = extract(&lexer);
		tokens[i] = token;
	}

	return tokens;
}

static Token extract(Lexer *lexer) {

	while (match_space(lexer))
		advance(lexer, 1);

	if (match_end(lexer))
		return (Token) {TOKEN_END, NULL};

	if (match_alpha(lexer))
		return extract_id(lexer);


	if (match_digit(lexer))
		return extract_num(lexer);

	return extract_op(lexer);
}

static Token extract_id(Lexer *lexer) {
	size_t start = lexer->index;
	while (match_alnum(lexer))
		advance(lexer, 1);
	
	char *id = strndup(lexer->input + start, lexer->index - start);
	return (Token) {TOKEN_ID, id};
}

static Token extract_num(Lexer *lexer) {
	size_t start = lexer->index;
	while (match_digit(lexer))
		advance(lexer, 1);

	if (match(lexer, '.')) {
		advance(lexer, 1);
		while (match_digit(lexer))
			advance(lexer, 1);
	}

	char *num = strndup(lexer->input + start, lexer->index - start);
	return (Token) {TOKEN_NUM, num};
}

static Token extract_op(Lexer *lexer) {
	// Operators are checked according to maximal munch principle
	// i.e. the longest possible match is chosen
	// This is why the order of operators in ops[] is important
	// e.g. "++" should be defined before "+"
	const static char *ops[] = { "+", "-", "*", "/", "^", ",", "(", ")" };
	const static TokenType types[] = { TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH, TOKEN_CARET, TOKEN_COMMA, TOKEN_LPAREN, TOKEN_RPAREN };
	const static size_t n = sizeof(ops) / sizeof(ops[0]);

	for (size_t i = 0; i < n; ++i) {
		const char *op = ops[i];
		size_t len = strlen(op);

		if (strncmp(lexer->input + lexer->index, op, len) == 0) {
			advance(lexer, len);
			return (Token) {types[i], strdup(op)};
		}
	}

	fprintf(stderr, "Unexpected character: %c\n", peek(lexer));
	exit(EXIT_FAILURE);
}

// Helper functions

static void advance(Lexer *lexer, size_t n) {
	lexer->index += n;
}

static char peek(Lexer *lexer) {
	return lexer->input[lexer->index];
}

static bool match(Lexer *lexer, char c) {
	return peek(lexer) == c;
}

static bool match_space(Lexer *lexer) {
	return isspace(peek(lexer));
}

static bool match_alpha(Lexer *lexer) {
	return isalpha(peek(lexer)) || peek(lexer) == '_';
}

static bool match_digit(Lexer *lexer) {
	return isdigit(peek(lexer));
}

static bool match_alnum(Lexer *lexer) {
	return match_alpha(lexer) || match_digit(lexer);
}

static bool match_end(Lexer *lexer) {
	return peek(lexer) == '\0';
}