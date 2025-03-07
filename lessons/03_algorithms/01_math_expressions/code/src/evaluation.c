#include "expression.h"

#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Variable related functions and data

static const Variable constants[] = {
	{ "pi", M_PI },
	{ "e", M_E }
};

static const size_t constant_count = sizeof(constants) / sizeof(Variable);

static double get_variable(const char *name, Variable *vars, size_t vcnt) {
	
	// Check if the variable is a constant
	for (size_t i = 0; i < constant_count; ++i) {
		if (strcmp(name, constants[i].name) == 0) {
			return constants[i].value;
		}
	}

	for (size_t i = 0; i < vcnt; ++i) {
		if (strcmp(name, vars[i].name) == 0) {
			return vars[i].value;
		}
	}

	return NAN;
}

// Function related functions and data

double my_abs(double *args, size_t argc) {
	return argc == 1 ? fabs(args[0]) : NAN;
}

double my_sin(double *args, size_t argc) {
	return argc == 1 ? sin(args[0]) : NAN;
}

double my_cos(double *args, size_t argc) {
	return argc == 1 ? cos(args[0]) : NAN;
}

double my_log(double *args, size_t argc) {
	return argc == 1 ? log(args[0]) : NAN;
}

double my_pow(double *args, size_t argc) {
	return argc == 2 ? pow(args[0], args[1]) : NAN;
}

// To add a new function, add a new entry to the builtins array and implement the function
static const Function builtins[] = {
	{ "abs", my_abs },
	{ "sin", my_sin },
	{ "cos", my_cos },
	{ "log", my_log },
	{ "pow", my_pow }
};

static const size_t builtin_count = sizeof(builtins) / sizeof(Function);

static double call_function(const char *name, double *args, size_t argc, Function *funcs, size_t fcnt) {
	
	// Check if the function is a builtin
	for (size_t i = 0; i < builtin_count; ++i) {
		if (strcmp(name, builtins[i].name) == 0) {
			return builtins[i].func(args, argc);
		}
	}
	
	for (size_t i = 0; i < fcnt; ++i) {
		if (strcmp(name, funcs[i].name) == 0) {
			return funcs[i].func(args, argc);
		}
	}
	
	return NAN;
}

// Operator related functions

static double apply_binary_operator(const char *op, double left, double right) {
	if (strcmp(op, "+") == 0) {
		return left + right;
	} else if (strcmp(op, "-") == 0) {
		return left - right;
	} else if (strcmp(op, "*") == 0) {
		return left * right;
	} else if (strcmp(op, "/") == 0) {
		return left / right;
	} else {
		return pow(left, right);
	}
}

static double apply_unary_operator(const char *op, double base) {
	if (strcmp(op, "-") == 0) {
		return -base;
	} else {
		return base;
	}
}

// Evaluation related functions

double evaluate_binary(ASTNode *, Variable *, size_t, Function *, size_t);
double evaluate_unary(ASTNode *, Variable *, size_t, Function *, size_t);
double evaluate_function(ASTNode *, Variable *, size_t, Function *, size_t);
double evaluate_variable(ASTNode *, Variable *, size_t, Function *, size_t);

double evaluate_expression(ASTNode *root, Variable *vars, size_t vcnt, Function *funcs, size_t fcnt) {
	if (root == NULL) return 0;

	switch (root->type) {
		case NODE_BINARY:
			return evaluate_binary(root, vars, vcnt, funcs, fcnt);
		case NODE_UNARY:
			return evaluate_unary(root, vars, vcnt, funcs, fcnt);
		case NODE_FUNC:
			return evaluate_function(root, vars, vcnt, funcs, fcnt);
		case NODE_GROUP:
			return evaluate_expression(root->group, vars, vcnt, funcs, fcnt);
		case NODE_VAR:
			return evaluate_variable(root, vars, vcnt, funcs, fcnt);
		default: // NODE_NUM
			return root->num;
	}
}

double evaluate_binary(ASTNode *root, Variable *vars, size_t vcnt, Function *funcs, size_t fcnt) {
	double left = evaluate_expression(root->binary.left, vars, vcnt, funcs, fcnt);
	double right = evaluate_expression(root->binary.right, vars, vcnt, funcs, fcnt);
	return apply_binary_operator(root->binary.op, left, right);
}

double evaluate_unary(ASTNode *root, Variable *vars, size_t vcnt, Function *funcs, size_t fcnt) {
	double base = evaluate_expression(root->unary.base, vars, vcnt, funcs, fcnt);
	return apply_unary_operator(root->unary.op, base);
}

double evaluate_function(ASTNode *root, Variable *vars, size_t vcnt, Function *funcs, size_t fcnt) {
	double args[root->func.argc];
	for (size_t i = 0; i < root->func.argc; ++i) {
		args[i] = evaluate_expression(root->func.args[i], vars, vcnt, funcs, fcnt);
	}

	double result = call_function(root->func.name, args, root->func.argc, funcs, fcnt);

	if (isnan(result)) {
		fprintf(stderr, "Error: Unknown function '%s'\n", root->func.name);
		exit(1);
	}
	return result;
}

double evaluate_variable(ASTNode *root, Variable *vars, size_t vcnt, Function *, size_t) {
	double value = get_variable(root->var, vars, vcnt);
	if (isnan(value)) {
		fprintf(stderr, "Error: Unknown variable '%s'\n", root->var);
		exit(1);
	}
	return value;
}