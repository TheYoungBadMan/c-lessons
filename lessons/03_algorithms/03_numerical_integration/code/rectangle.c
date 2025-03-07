#include <math.h>

#define MAX_ITERATIONS 64

// Single integral of f(x) from a to b using midpoint rectangle rule with n intervals
// x*_i - midpoint of the i-th interval [x_i, x_{i+1}], x*_i = (x_i + x_{i+1}) / 2
double rectangle(double a, double b, unsigned long long n, double (*f)(double)) {
	if (n == 0) return 0.;

	double dx = (b - a) / n;
	double I = 0.;
	for (unsigned long long i = 0; i < n; ++i) {
		I += f(a + (i + 0.5) * dx);
	}
	return dx * I;
}

// Single integral of f(x) from a to b using adaptive midpoint rectangle rule
// e.g. integral value is calculated until the difference between two consecutive values is less than eps
double rectangle_adaptive(double a, double b, double eps, double (*f)(double)) {
	double I_new, I_old = 0.;
	unsigned long long n = 1;
	for (int i = 0; i < MAX_ITERATIONS; ++i, n *= 2) {
		I_new = rectangle(a, b, n, f);
		if (fabs(I_new - I_old) < eps) {
			break;
		}
		I_old = I_new;
	}
	return I_new;
}

// Single integral of f(x) from a to b using any-point rectangle rule with array of values of f
// n is the number of intervals, f[0] = f(a), f[n] = f(b)
// f[i] = f(x*_i), x*_i - any point in the i-th interval [x_i, x_{i+1}]
double rectangle_array(double a, double b, unsigned long long n, double *f) {
	if (n == 0) return 0.;

	double dx = (b - a) / n;
	double I = 0.;
	for (unsigned long long i = 0; i < n; ++i) {
		I += f[i];
	}
	return dx * I;
}

