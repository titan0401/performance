#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

int main()
{
// get dimension
int n;
printf("Please provide matrix dimension n: ");
scanf("\%d",&n);
const int stride = n + 20;
//alloc matrices 
double *A = malloc(n*stride*sizeof(double));
double *B = malloc(n*stride*sizeof(double));
double *C = malloc(n*stride*sizeof(double));

//fill with 2 on the diagonal and 1 everywhere else 
for(int i = 0; i <n; i++){
    for(int j = 0; j < n; j++){
            A[i+j*stride] = (i==j) ? 2 : 1;
        }
}

//fill with 1/n
for(int i = 0; i < n; i++){
    for(int j = 0; j < n; j++){
		B[i+j*stride] = j*1./n;
    }
}

// print matrices if n is less or equal 32
if(n <= 32){
    printf("Matrix A:\n");
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
                printf("  %g",A[i+j*stride]);
            }
            printf("\n");
    }
    printf("Matrix B:\n");
    for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                printf("  %g",B[i+j*stride]);
            }
            printf("\n");
    }
}

// calculate matrix-matrix product
double wtime = omp_get_wtime();
for(int i = 0; i < n; i+=8){
    for(int j = 0; j < n; j++){
		double cij0 = 0;
		double cij1 = 0;
		double cij2 = 0;
		double cij3 = 0;
		double cij4 = 0;
		double cij5 = 0;
		double cij6 = 0;
		double cij7 = 0;		
        for(int k = 0; k < n; k++){
            cij0 += A[i+0+k*n] * B[k+j*n];
			cij1 += A[i+1+k*n] * B[k+j*n];
			cij2 += A[i+2+k*n] * B[k+j*n];
			cij3 += A[i+3+k*n] * B[k+j*n];
			cij4 += A[i+4+k*n] * B[k+j*n];
			cij5 += A[i+5+k*n] * B[k+j*n];
			cij6 += A[i+6+k*n] * B[k+j*n];
			cij7 += A[i+7+k*n] * B[k+j*n];
        }
		C[i+0+j*n] = cij0;
		C[i+1+j*n] = cij1;
		C[i+2+j*n] = cij2;
		C[i+3+j*n] = cij3;
		C[i+4+j*n] = cij4;
		C[i+5+j*n] = cij5;
		C[i+6+j*n] = cij6;
		C[i+7+j*n] = cij7;
    }
}
wtime = omp_get_wtime() - wtime;
printf("Runtime: %g, performance: %g [GFlop/s]\n", wtime, n/1000000000./wtime);

// print result if n is less or equal 32
if(n <= 32){
    printf("Matrix C:\n");
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            printf("  %g",C[i+j*stride]);
        }
        printf("\n");
    }
}

free(A);
free(B);
free(C);

return 0;
}
