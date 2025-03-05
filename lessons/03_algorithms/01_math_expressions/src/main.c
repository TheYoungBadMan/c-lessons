#include "expression.h"

#include <stdio.h>

double my_function(double *args, size_t) {
	return args[0] * args[0];
}

int main() {
	ASTNode *root = create_expression("1 + 2 * x + f(4)");
	print_expression(root);

	Variable x = { .name = "x", .value = 3 };
	Function f = { .name = "f", .func = my_function };

	printf("result = %f\n", evaluate_expression(root, &x, 1, &f, 1));

	delete_expression(root);
	return 0;
}