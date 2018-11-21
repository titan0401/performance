#define _GNU_SOURCE
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <sched.h>
#include <emmintrin.h>

int main()
{
	const int nb=2;
	int n;
	double M[nb][nb];
	printf("Number of rows n: ");
	scanf("%d", &n);
	//double *V = malloc(nb * n * sizeof(double));
	//double *W = malloc(nb * n * sizeof(double));
	double *V = NULL; posix_memalign((void**)&V, 16, nb * n * sizeof(double));
	double *W = NULL; posix_memalign((void**)&W, 16, nb * n * sizeof(double));
	#pragma omp parallel
	{
	//	printf("Hello OpenMP from thread %d / %d on core %d\n", omp_get_thread_num(), omp_get_num_threads(), sched_getcpu());
	}
	
	
	
	

	
	#pragma omp parallel for schedule(static)
	for (int i = 0; i<n; i++)
		{
			for(int j = 0; j<nb; j++)
			{
			V[i*nb+j] = (double)(j+1);
			}

		}
		
		
	for(int i=0;i<nb;i++)
	{
		for(int j=0;j<nb;j++)
		{
			M[j][i]= (double)(i*nb+j);
		}
	}
	
			//Print if n<10
		if(n<10)
		{
			printf("Since n < 10, matrices are printed:\n");
			printf("V:\n");
			for(int i = 0; i<n; i++)
			{
				for(int j = 0; j<nb; j++)
				{
					printf(" %g",V[i*nb+j]);
				}
				printf("\n");
			}
			
			printf("M:\n");
			for(int i = 0; i<nb; i++)
			{
				for(int j = 0; j<nb; j++)
				{
					printf(" %g",M[j][i]);
				}
				printf("\n");
			}
		}
	
		
		const int nIter = 100;
		
		double wtime = omp_get_wtime();
		for(int iter = 0; iter < nIter; iter ++)
		{
	
	#pragma omp parallel for schedule(static)	
		for(int i = 0; i<n ;i++)
		{
			//for(int j = 0; j<nb ;j++)
			//{
				//W[i*nb+j] = 0; //overwriting result
				//for(int k = 0; k<nb; k++)
				//{
					//W[i*nb+j] += (double)(V[i*nb+k]*M[j][k]);
					__m128d vi1 = _mm_load_pd1(&V[i]);
					__m128d m1 = _mm_set_pd(M[0][1],M[0][0]);
					__m128d vi1m1 = _mm_mul_pd(vi1,m1);
					__m128d vi2 = _mm_load_pd1(&V[i+1]);
					__m128d m2 = _mm_set_pd(M[1][1],M[1][0]);
					__m128d vi2m2 = _mm_mul_pd(vi2,m2);
					__m128d wi = _mm_add_pd(vi1m1,vi2m2);
					_mm_stream_pd(&W[nb*i], wi);
				//}
			//}
		}	
		}
		wtime = omp_get_wtime() - wtime;
		
		if ( n < 10)
		{
			printf("W:\n");
			for(int i = 0; i<n; i++)
			{
				for(int j = 0; j<nb; j++)
				{
					printf(" %g",W[i*nb+j]);
				}
				printf("\n");
			}
		}
		printf("Runtime: %g s\n", wtime);
		printf("Performance: %g GFlop/s\n", 2.*n*nb*nb*nIter/wtime*1./1000*1./1000*1./1000);
		printf("Bandwith: %g Gbytes/s\n", 8.*2*nb*n*nIter/wtime*1./1000*1./1000*1./1000);


		
		free(V);
		free(W);
		return 0;
}

