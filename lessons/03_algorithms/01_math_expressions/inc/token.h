#pragma once

typedef enum {
	TOKEN_ID, TOKEN_NUM,
	TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH, TOKEN_CARET,
	TOKEN_COMMA, TOKEN_LPAREN, TOKEN_RPAREN,
	TOKEN_END
} TokenType;

typedef struct {
	TokenType type;
	char *value;
} Token;

