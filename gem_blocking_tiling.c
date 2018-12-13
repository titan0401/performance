#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>
#define BLOCK 16

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

//define blocks
double tmpA[BLOCK*BLOCK];
double tmpB[BLOCK*BLOCK];
double tmpC[BLOCK*BLOCK];

// calculate matrix-matrix product
double wtime = omp_get_wtime();
for(int j = 0; j < n; j+=BLOCK){
	for(int i = 0; i < n; i+=BLOCK){
		
		// set tmpC to zero
		for(int jb = 0; jb < BLOCK; jb++){
			for(int ib = 0; ib < BLOCK; ib++){
				tmpC[ib+jb*BLOCK] = 0;
			}
		}
		
		for(int k = 0; k < n; k+=BLOCK){
			
			//get block of A
			for(int kb = 0; kb < BLOCK; kb++){
				for( int ib = 0; ib < BLOCK; ib++){
					tmpA[ib+kb*BLOCK] = A[i+ib+(k+kb)*n];
				}
			}
			
			//get block of b
			for(int jb = 0; jb < BLOCK; jb++){
				for(int kb = 0; kb < BLOCK; kb++){
					tmpB[kb+jb*BLOCK] = B[k+kb+(j+jb)*n];
				}
			}
			
			//do block
			for(int ib = 0; ib < BLOCK; ib++){
				for(int jb = 0; jb < BLOCK; jb++){
					double cij = tmpC[ib+jb*BLOCK];
					for(int kb = 0; kb < BLOCK; kb++){
						cij += tmpA[ib+kb*BLOCK] * tmpB[kb+jb*BLOCK];
					}
					tmpC[ib+jb*BLOCK] += cij;
				}
			}
		}
		// store block in C
		for(int jb = 0; jb < BLOCK; jb++){
			for(int ib = 0; ib < BLOCK; ib++){
				C[i+ib+(j+jb)*n] = tmpC[ib+jb*BLOCK];
			}
		}
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
