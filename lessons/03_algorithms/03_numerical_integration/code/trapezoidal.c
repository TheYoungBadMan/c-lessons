#include <math.h>

#define MAX_ITERATIONS 64

// Single integral of f(x) from a to b using trapezoidal rule with n intervals
double trapezoidal(double a, double b, unsigned long long n, double (*f)(double)) {
	if (n == 0) return 0.;

	double dx = (b - a) / n;
	double I = 0.;
	for (unsigned long long i = 1; i < n; ++i) {
		I += f(a + i * dx);
	}
	return ((f(a) + f(b)) / 2 + I) * dx;
}

// Single integral of f(x) from a to b using adaptive trapezoidal rule
// e.g. integral value is calculated until the difference between two consecutive values is less than eps
double trapezoidal_adaptive(double a, double b, double eps, double (*f)(double)) {
	double I_new, I_old = 0.;
	unsigned long long n = 1;
	for (int i = 0; i < MAX_ITERATIONS; ++i, n *= 2) {
		I_new = trapezoidal(a, b, n, f);
		if (fabs(I_new - I_old) < eps) {
			break;
		}
		I_old = I_new;
	}
	return I_new;
}

// Single integral of f(x) from a to b using optimized adaptive trapezoidal rule
// e.g. integral value is calculated until the difference between two consecutive values is less than eps
// each iteration calculates the integral with twice the number of intervals but only half of the function values are recalculated
// because after doubling the number of intervals, previous odd and even values become even values
// and only new odd values are recalculated
double trapezoidal_optimized(double a, double b, double eps, double (*f)(double)) {
	double dx = b - a;
	double I_new, I_old = 0., I_tmp = 0.;
	unsigned long long n = 1;
	for (int i = 0; i < MAX_ITERATIONS; ++i, n *= 2, dx /= 2) {
		for (unsigned long long j = 1; j < n; j += 2) {
			I_tmp += f(a + j * dx);
		}

		I_new = ((f(a) + f(b)) / 2 + I_tmp) * dx;

		if (fabs(I_new - I_old) < eps) {
			break;
		}
		I_old = I_new;
	}
	return I_new;
}

// Single integral of f(x) from a to b using trapezoidal rule with array of values of f
// n is the number of intervals, f[0] = f(a), f[n] = f(b)
// f[i] = f(x_i), x_i = a + i * dx, dx = (b - a) / n
double trapezoidal_array(double a, double b, unsigned long long n, double *f) {
	if (n == 0) return 0.;

	double dx = (b - a) / n;
	double I = (f[0] + f[n]) / 2;
	for (unsigned long long i = 1; i < n; ++i) {
		I += f[i];
	}
	return dx * I;
}