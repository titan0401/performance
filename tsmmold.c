#define _GNU_SOURCE
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <sched.h>

int main()
{
  const int nb = 2;
  const int n = 10000000;
  //printf("Number of rows: ");
  //scanf("%d", &n);
  double *V = malloc(nb * n*sizeof(double));
  double *W = malloc(nb * n*sizeof(double));
  double M[nb][nb];

#pragma omp parallel
  {
    printf("OpenMP thread %d / %d on core %d\n", omp_get_thread_num(), omp_get_num_threads(), sched_getcpu());
  }

  // initialize on correct NUMA domain
#pragma omp parallel for schedule(static)
  for(int i = 0; i < n; i++)
  {
    for(int j = 0; j < nb; j++)
      V[i*nb+j] = j+1;
  }

  for(int i = 0; i < nb; i++)
    for(int j = 0; j < nb; j++)
      M[j][i] = i*nb+j;

  if( n < 10 )
  {
    printf("V:\n");
    for(int i = 0; i < n; i++)
    {
       for(int j = 0; j < nb; j++)
         printf(" %g", V[i*nb+j]);
       printf("\n");
    }
    printf("M:\n");
    for(int i = 0; i < nb; i++)
    {
       for(int j = 0; j < nb; j++)
         printf(" %g", M[j][i]);
       printf("\n");
    }
  }
  const int nIter = 100;

  double wtime = omp_get_wtime();
  for(int iter = 0; iter < nIter; iter++)
  {
// omit firstprivate(M) because it somehow reduces the performance!
#pragma omp parallel for schedule(static)
    for(int i = 0; i < n; i++)
    {
      for(int j = 0; j < nb; j++)
      {
        W[i*nb+j] = 0;
        for(int k = 0; k < nb; k++)
          W[i*nb+j] += V[i*nb+k] * M[j][k];
      }
    }
  }
  wtime = omp_get_wtime() - wtime;

  if( n < 10 )
  {
    printf("W:\n");
    for(int i = 0; i < n; i++)
    {
       for(int j = 0; j < nb; j++)
         printf(" %g", W[i*nb+j]);
       printf("\n");
    }
  }
  printf("Runtime: %g s\n", wtime);
  printf("Performance: %g GFlop/s\n", 2.*n*nb*nb*nIter/wtime*1./1000*1./1000*1./1000);
  printf("Bandwidth: %g Gbytes/s\n", 8.*2*nb*n*nIter/wtime*1./1000*1./1000*1./1000);

  return 0;
}