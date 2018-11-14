#define _GNU_SOURCE
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <sched.h>

int main()
{
	#pragma omp parallel
	{
		printf("Hello OpenMP from thread %d / %d on core %d\n", omp_get_thread_num(), omp_get_num_threads(), sched_getcpu());
	}
	
	int nb=2;
	int n;
	double M[nb][nb];
	printf("Number of rows n: ");
	scanf("%d", &n);
	double *V = malloc(n*nb*sizeof(double));
	double *W = malloc(n*nb*sizeof(double));
	
	//Initialize M
	for(int i=0;i<nb;i++)
	{
		for(int j=0;j<nb;j++)
		{
			M[i][j]= double(i*nb+j);
		}
	}
	
	// Initialize V
	
	#pragma omp parallel for schedule(static)
		for (int i = 0; i<n; i++)
		{
			for(int j = 0; j<nb; j++)
			{
			V[i][j] = double(j+1);
			}
		}
		
		const int nIter = 100;
		
		double wtime = omp_get_wtime();
		for(int iter = 0; iter < nIter; iter ++)
		{
	
	#pragma omp parallel for schedule(static) firstprivate(M)	
		for(int i = 0; i<n ;i++)
		{
			for(int j = 0; j<nb ;j++)
			{
				W[i][j] = 0; //overwriting result
				for(int k = 0; k<nb; k++)
				{
					W[i][j] += double(V[i][k]*M[k][j]);
				}
			}
		}	
		}
		wtime = omp_get_wtime() - wtime;
		
		printf("Runtime: %g s\n", wtime);
		printf("Performance: %g GFlop/s\n", 2.*n*nb*(nb-1)*nIter/wtime*1./1000*1./1000*1./1000);
		printf("Performance: %g Gbytes/s\n", 8.*nb*n*nIter/wtime*1./1000*1./1000*1./1000);

		//Print if n<10
		if(n<10)
		{
			printf("Since n < 10, matrices are printed:\n");
			printf("V:\n");
			for(int i = 0; i<n; i++)
			{
				for(int j = 0; j<nb; j++)
				{
					printf("%d ",V[i][j]);
				}
				printf("\n");
			}
			
			printf("M:\n");
			for(int i = 0; i<nb; i++)
			{
				for(int j = 0; j<nb; j++)
				{
					printf("%d ",M[i][j]);
				}
				printf("\n");
			}
			
			printf("W:\n");
			for(int i = 0; i<n; i++)
			{
				for(int j = 0; j<nb; j++)
				{
					printf("%d ",W[i][j]);
				}
				printf("\n");
			}
		}
		
		free(V);
		free(W);
		return 0;
}