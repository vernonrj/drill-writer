// compile with:
// $ gcc $(gsl-config --cflags) perf_spline.c $(gsl-config --libs)
// view with graph by doing
// $ ./a.out > interp.dat
// $ graph -T ps < interp.dat > interp.ps

// create a periodic spline

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

int
main (void)
{
	int N = 4;
	double x[5] = {0.00, 0.10,  0.27,  0.40};
	double y[5] = {0.15, 0.70, 0.65,  0.35}; 
		   /* Note: y[0] == y[3] for periodic data */

	gsl_interp_accel *acc = gsl_interp_accel_alloc ();

	const gsl_interp_type *t = gsl_interp_cspline_periodic; 
	//const gsl_interp_type *t = gsl_interp_cspline;

	gsl_spline *spline = gsl_spline_alloc (t, N);

	int i; double xi, yi;

	printf ("#m=0,S=5\n");
	for (i = 0; i < N; i++)
	{
		printf ("%g %g\n", x[i], y[i]);
	}

	printf ("#m=1,S=0\n");
	gsl_spline_init (spline, x, y, N);

	for (i = 0; i <= 100; i++)
	{
		xi = (1 - i / 100.0) * x[0] + (i / 100.0) * x[N-1];
		yi = gsl_spline_eval (spline, xi, acc);
		printf ("%g %g\n", xi, yi);
	}

	gsl_spline_free (spline);
	gsl_interp_accel_free (acc);
	return 0;
}
