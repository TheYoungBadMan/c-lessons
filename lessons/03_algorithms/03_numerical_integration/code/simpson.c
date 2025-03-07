#define MAX_ITERATIONS 64

// Single integral of f(x) from a to b using Simpson's rule with n intervals
double simpson(double a, double b, unsigned long long n, double (*f)(double)) {
	if (n == 0) return 0.;
	double dx = (b - a) / n;
	double I_odd = f(a + dx / 2), I_even = 0;
	for (unsigned long long i = 1; i < n; ++i) {
		I_even += f(a + i * dx);
		I_odd += f(a + (i + 0.5) * dx);
	}
	return (f(a) + 2 * I_even + 4 * I_odd + f(b)) * dx / 6;
}

// Single integral of f(x) from a to b using adaptive Simpson's rule
// e.g. integral value is calculated until the difference between two consecutive values is less than eps
double simpson_adaptive(double a, double b, double eps, double (*f)(double)) {
	double I_new, I_old = 0.;
	unsigned long long n = 1;
	for (int i = 0; i < MAX_ITERATIONS; ++i, n *= 2) {
		I_new = simpson(a, b, n, f);
		if (fabs(I_new - I_old) < eps) {
			break;
		}
		I_old = I_new;
	}
	return I_new;
}

// Single integral of f(x) from a to b using optimized adaptive Simpson's rule
// e.g. integral value is calculated until the difference between two consecutive values is less than eps
// each iteration calculates the integral with twice the number of intervals but only half of the function values are recalculated
// because after doubling the number of intervals, previous odd and even values become even values
// and only new odd values are recalculated
double simpson_optimized(double a, double b, double eps, double (*f)(double)) {
	double dx = (b - a);
	double I_new, I_old = 0., I_even = 0., I_odd = 0.;
	unsigned long long n = 1;
	for (int i = 0; i < MAX_ITERATIONS; ++i, n *= 2, dx /= 2) {
		I_even += I_odd;
		I_odd = 0.;
		for (unsigned long long j = 0; j < n; ++j) {
			I_odd += f(a + (j + 0.5) * dx);
		}
		I_new = (f(a) + 2 * I_even + 4 * I_odd + f(b)) * dx / 6;

		if (fabs(I_new - I_old) < eps) {
			break;
		}
		I_old = I_new;
	}
	return I_new;
}

// Single integral of f(x) from a to b using Simpson's rule with array of values of f
// n is the even number of intervals, f[0] = f(a), f[n] = f(b)
// f[i] = f(a + i * dx), dx = (b - a) / n
double simpson_array(double a, double b, unsigned long long n, double *f) {
	if (n == 0) return 0.;
	
	double dx = (b - a) / n;
	double I_odd = f[1], I_even = 0;
	for (unsigned long long i = 2; i < n; i += 2) {
		I_even += f[i];
		I_odd += f[i + 1];
	}
	return (f[0] + 2 * I_even + 4 * I_odd + f[n]) * dx / 3;
}