#define _GNU_SOURCE
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <sched.h>

int main()
{
	int n;
	printf("Vector dimension: ");
	scanf("%d", &n);
	double *x = malloc(n*sizeof(double));
	double *y = malloc(n*sizeof(double));

	#pragma omp parallel
	{
		printf("Hello OpenMP from thread %d / %d on core %d\n", omp_get_thread_num(), omp_get_num_threads(), sched_getcpu());
	}
	
	// exploit sum_(k=1)^n 1/(k*k+1)) = 1 - 1/(n+1)
	
	#pragma omp parallel for schedule(static)
		for (int i = 0; i<n; i++)
		{
			x[i] = 1./(i+1);
			y[i] = 1./(i+2);
		}
		
		const int nIter = 100;
		
		double result = 0;
		
		double wtime = omp_get_wtime();
		for(int iter = 0; iter < nIter; iter ++)
		{
			double dot = 0;
	#pragma omp parallel for schedule(static) reduction(+:dot)
		for(int i= 0; i<n; i++)
			dot +=x[i]*y[i];
		result += dot;
		}
		wtime = omp_get_wtime() - wtime;
		
		printf("Error: %g\n", fabs(result-nIter*(1-1./(n+1))));
		printf("Runtime: %g s\n", wtime);
		printf("Performance: %g GFlop/s\n", 2.*n*nIter/wtime*1./1000*1./1000*1./1000);
		printf("Performance: %g Gbytes/s\n", 8.*2*n*nIter/wtime*1./1000*1./1000*1./1000);
		
		return 0;
}
	
	
	
	
	
	
	
	
	
}