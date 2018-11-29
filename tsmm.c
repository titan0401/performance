/** @file "tsmm.c"
 * @brief OpenMP matrix-matrix product with a double-precision tall-skinny (W <- V M) using SIMD intrinsics
 * @author Melven.Roehrig-Zoellner@DLR.de
 *
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <sched.h>
#include <emmintrin.h>

int main()
{
  const int nb = 8;
  const int n = 5;//1000000;
  printf("Number of rows: %d\n", n);
  //scanf("%d", &n);
  double *V = NULL;
  posix_memalign((void**)&V, 16, nb * n*sizeof(double));
  double *W = NULL;
  posix_memalign((void**)&W, 16, nb * n*sizeof(double));
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
      M[i][j] = i*nb+j;

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
         printf(" %g", M[i][j]);
       printf("\n");
    }
  }
  const int nIter = 100;

  double wtime = omp_get_wtime();
  for(int iter = 0; iter < nIter; iter++)
  {
    if( nb == 2 )
    {
      __m128d m1 = _mm_set_pd(M[0][1],M[0][0]); // set first row of M
      __m128d m2 = _mm_set_pd(M[1][1],M[1][0]); // set second row of M

#pragma omp parallel for schedule(static)
      for(int i = 0; i < n; i++)
      {
        __m128d vi1 = _mm_load_pd1(&V[2*i]);      // load V_i1 into both SSE elements
        __m128d vi1m1 = _mm_mul_pd(vi1,m1);       // multiply V_i1 with row of M
        __m128d vi2 = _mm_load_pd1(&V[2*i+1]);    // load V(i,2) into both SSE elements
        __m128d vi2m2 = _mm_mul_pd(vi2,m2);       // multiply V_i2 with row of M
        __m128d wi = _mm_add_pd(vi1m1,vi2m2);     // add up row of W
        _mm_stream_pd(&W[2*i], wi);               // write result to W using non-temporal stores
      }
    }
    else if( nb == 4 )
    {
      // set M, should be optimized away by the compiler so M is kept in a register by the CPU
      __m128d m11 = _mm_set_pd(M[0][1], M[0][0]);
      __m128d m12 = _mm_set_pd(M[0][3], M[0][2]);
      __m128d m21 = _mm_set_pd(M[1][1], M[1][0]);
      __m128d m22 = _mm_set_pd(M[1][3], M[1][2]);
      __m128d m31 = _mm_set_pd(M[2][1], M[2][0]);
      __m128d m32 = _mm_set_pd(M[2][3], M[2][2]);
      __m128d m41 = _mm_set_pd(M[3][1], M[3][0]);
      __m128d m42 = _mm_set_pd(M[3][3], M[3][2]);

#pragma omp parallel for schedule(static)
      for(int i = 0; i < n; i++)
      {
        // load row of V
        __m128d vi1 = _mm_load_pd1(&V[4*i+0]);
        __m128d vi2 = _mm_load_pd1(&V[4*i+1]);
        __m128d vi3 = _mm_load_pd1(&V[4*i+2]);
        __m128d vi4 = _mm_load_pd1(&V[4*i+3]);

        // multiplications
        __m128d vi1m11 = _mm_mul_pd(vi1,m11);
        __m128d vi1m12 = _mm_mul_pd(vi1,m12);
        __m128d vi2m21 = _mm_mul_pd(vi2,m21);
        __m128d vi2m22 = _mm_mul_pd(vi2,m22);
        __m128d vi3m31 = _mm_mul_pd(vi3,m31);
        __m128d vi3m32 = _mm_mul_pd(vi3,m32);
        __m128d vi4m41 = _mm_mul_pd(vi4,m41);
        __m128d vi4m42 = _mm_mul_pd(vi4,m42);

        // add everything up again
        __m128d w12_1 = _mm_add_pd(vi1m11, vi2m21);
        __m128d w12_2 = _mm_add_pd(vi3m31, vi4m41);

        __m128d w34_1 = _mm_add_pd(vi1m12, vi2m22);
        __m128d w34_2 = _mm_add_pd(vi3m32, vi4m42);

        __m128d w12 = _mm_add_pd(w12_1, w12_2);
        __m128d w34 = _mm_add_pd(w34_1, w34_2);

        // non-temporal store for row of W
        _mm_stream_pd(&W[4*i+0], w12);
        _mm_stream_pd(&W[4*i+2], w34);
      }
    }
    else if( nb == 8 )
    {
      // nicer memory layout for M
      __m128d mi[4][8];
      for(int i = 0; i < 4; i++)
        for(int j = 0; j < 8; j++)
          mi[i][j] = _mm_set_pd(M[j][i+1], M[j][i]);
#pragma omp parallel for schedule(static)
      for(int i = 0; i < n; i++)
      {
        // handle two columns of M at once
        for(int j = 0; j < 8; j+=2)
        {
          // multiply with entries of M
          __m128d vimi[8];
          for(int k = 0; k < 8; k++)
            vimi[k] = _mm_mul_pd(_mm_set1_pd(V[8*i+k]), mi[j/2][k]);
          // add up result (in a tree-wise manner)
          __m128d wi4[4];
          for(int k = 0; k < 4; k++)
            wi4[k] = _mm_add_pd(vimi[2*k+1], vimi[2*k]);
          __m128d wi2[2];
          for(int k = 0; k < 2; k++)
            wi2[k] = _mm_add_pd(wi4[2*k+1], wi4[2*k]);
          __m128d wi = _mm_add_pd(wi2[1], wi2[0]);

          // non-temporal store to W
          _mm_stream_pd(&W[8*i+j], wi);
        }
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
  printf("Performance: %g Gbytes/s\n", 8.*2*nb*n*nIter/wtime*1./1000*1./1000*1./1000);

  return 0;
}
