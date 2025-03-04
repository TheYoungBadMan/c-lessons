#include "regex.h"

#include "ast.h"

#include <stdlib.h>
#include <stdbool.h>

ASTNode *parse(const char *);

ASTNode *create_regex(const char *regex) {
	return parse(regex);
}

void delete_regex(ASTNode *regex) {
	delete_ast(regex);
}

// Searches for the first match of the regex in the string starting from the given position.
// If a match is found, pos is updated to the position where the match starts and len is updated to the length of the match.
void match_first(ASTNode *regex, const char *str, size_t *pos, size_t *len) {

}

// Searches for all matches of the regex in the string. 
// cnt is updated to the number of matches found.
// pos is updated to an array of positions where the matches start.
// len is updated to an array of lengths of the matches.
void match_all(ASTNode *regex, const char *str, size_t *cnt, size_t **pos, size_t **len) {
}

// Returns true if the regex matches the entire string.
bool match_full(ASTNode *regex, const char *str) {
}

// Main matching function.

bool match_regex(ASTNode *, const char *, size_t, size_t*);
bool match_alter(ASTNode *, const char *, size_t, size_t*);
bool match_concat(ASTNode *, const char *, size_t, size_t*);
bool match_quantifier(ASTNode *, const char *, size_t, size_t*);
bool match_anchor(ASTNode *, const char *, size_t, size_t*);
bool match_char_class(ASTNode *, const char *, size_t, size_t*);
bool match_custom_class(ASTNode *, const char *, size_t, size_t*);
bool match_group(ASTNode *, const char *, size_t, size_t*);
bool match_char(ASTNode *, const char *, size_t, size_t*);

bool match_regex(ASTNode *node, const char *str, size_t pos, size_t *len) {
	switch (node->type) {
		case NODE_ALTER:
			return match_alter(node, str, pos, len);
		case NODE_CONCAT:
			return match_concat(node, str, pos, len);
		case NODE_QUANTIFIER:
			return match_quantifier(node, str, pos, len);
		case NODE_ANCHOR:
			return match_anchor(node, str, pos, len);
		case NODE_CHAR_CLASS:
			return match_char_class(node, str, pos, len);
		case NODE_CUSTOM_CLASS:
			return match_custom_class(node, str, pos, len);
		case NODE_GROUP:
			return match_group(node, str, pos, len);
		default: // NODE_CHAR
			return match_char(node, str, pos, len);
	}
}

bool match_alter(ASTNode *node, const char *str, size_t pos, size_t *len) {
	return match_regex(node->binary.left, str, pos, len) || match_regex(node->binary.right, str, pos, len);
}

bool match_concat(ASTNode *node, const char *str, size_t pos, size_t *len) {
	size_t l = 0;
	if (match_regex(node->binary.left, str, pos, &l)) {
		return match_regex(node->binary.right, str, pos + l, len);
	}
	return 0;
}

bool match_quantifier(ASTNode *node, const char *str, size_t pos, size_t *len) {
	int min = node->quantifier.min;
	int max = node->quantifier.max;
	size_t total_len = 0;
	size_t match_len;

	for (int i = 0; i < min; i++) {
		if (!match_regex(node->quantifier.base, str, pos + total_len, &match_len)) {
			return 0;
		}
		total_len += match_len;
	}

	if (max == -1) {
		while (match_regex(node->quantifier.base, str, pos + total_len, &match_len)) {
			total_len += match_len;
		}
	} else {
		for (int i = min; i < max; i++) {
			if (!match_regex(node->quantifier.base, str, pos + total_len, &match_len)) {
				break;
			}
			total_len += match_len;
		}
	}

	*len = total_len;
	return 1;
		
}

bool match_anchor(ASTNode *node, const char *str, size_t pos, size_t *len) {
	switch (node->anchor) {
		case ANCHOR_START:
			if (pos == 0) {
				*len = 0;
				return 1;
			}
			break;
		case ANCHOR_END:
			if (str[pos] == '\0') {
				*len = 0;
				return 1;
			}
			break;
		case ANCHOR_WORD:
			bool left = (pos > 0) && (isalnum(str[pos - 1]) || str[pos - 1] == '_');
			bool right = (str[pos] != '\0') && (isalnum(str[pos]) || str[pos] == '_');
			if (left != right) {
				*len = 0;
				return 1;
			}
			break;
		default: // ANCHOR_NON_WORD
			bool left = (pos > 0) && !(isalnum(str[pos - 1]) || str[pos - 1] == '_');
			bool right = (str[pos] != '\0') && !(isalnum(str[pos]) || str[pos] == '_');
			if (left != right) {
				*len = 0;
				return 1;
			}
	}
}

bool match_char_class(ASTNode *node, const char *str, size_t pos, size_t *len) {
	bool match = 0;

	switch (node->char_class.type) {
		case CLASS_DIGIT:
			match = isdigit(str[pos]); break;
		case CLASS_WORD:
			match = isalnum(str[pos]) || str[pos] == '_'; break;
		case CLASS_SPACE:
			match = isspace(str[pos]); break;
		default: // CLASS_DOT
			match = str[pos] != '\0' && str[pos] != '\n';
	}

	if (node->char_class.negated) {
		match = !match;
	}

	if (match) {
		*len = 1;
		return 1;
	}

	return 0;
}

bool match_custom_class(ASTNode *node, const char *str, size_t pos, size_t *len) {
	bool match = 0;

	for (size_t i = 0; i < node->custom_class.cnt; i++) {
		if (node->custom_class.ranges[i].start <= str[pos] && str[pos] <= node->custom_class.ranges[i].end) {
			match = 1;
			break;
		}
	}

	if (node->custom_class.negated) {
		match = !match;
	}

	if (match) {
		*len = 1;
		return 1;
	}

	return 0;
}

bool match_group(ASTNode *node, const char *str, size_t pos, size_t *len) {
	return match_regex(node->group, str, pos, len);
}

bool match_char(ASTNode *node, const char *str, size_t pos, size_t *len) {
	if (str[pos] == node->value) {
		*len = 1;
		return 1;
	}
	return 0;
}
