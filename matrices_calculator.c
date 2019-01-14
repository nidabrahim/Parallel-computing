/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "omp.h"


// taille maximum des matrices statiques sur etud avec 48 threads (par défaut)
// #define SIZE 590 
#define SHOWSIZE 10 

#define DUMP(flux, ch, x) fprintf(flux, "%s %s\n", ch, #x);
  
void showMatrice(double **A, int n)
{
	int i, j;
	int mytid=-1, nb_ths=0;
	
	for (i=0; i<n; i++) {
		for (j=0; j<n; j++)
			printf("%6.2f%c", A[i][j], ((j+1)%SHOWSIZE) ? '\t' : '\n');  
		printf("\n");  
	}
}


double ** allocMatrice(int n)
{
	double **M=NULL;
	int i, j;

	M = (double **)malloc(n * sizeof(double *));
	if (M) {
		for (i=0; i<n; i++) {
			M[i] = (double *)malloc(n * sizeof(double)); 
			if (!(M[i])) {
				for (j=i-1; j>=0; j--) free(M[j]);
				free(M);
				M = NULL;
				break;
			}
		}
	}

	return M;
}

double ** freeMatrice(double **A, int n)
{
	int i;

	for (i=n-1; i>=0; i--)
		free(A[i]);
	free(A);
	A = NULL;

	return A;
}

int main(int argc, char **argv)
{
	double **A=NULL, **B=NULL, **C=NULL;

	int i, j, k;
	
	double tstart=0., texec=0.;

	int SIZE=100;

	if (argc > 1) SIZE = atoi(argv[1]);

	/* Allocation de 3 matrices */
	A=allocMatrice(SIZE);	
	if (A) {
		B=allocMatrice(SIZE);	
		if (B) C=allocMatrice(SIZE);
		else A=freeMatrice(A, SIZE);
	}
	if (!C) {
		B = freeMatrice(B, SIZE);
		A = freeMatrice(A, SIZE);

		return -1;
	}

	tstart = omp_get_wtime();
	#pragma omp parallel default(shared) private(i,j,k)
	{
		//printf("Initialisation des matrices ...\n");
		#pragma omp parallel for shared(A, B) private(i,j)
		for (i=0; i<SIZE; i++) {
			for (j=0; j<SIZE; j++) {
				A[i][j] = i*SIZE+j; 
				B[i][j] = i+j; 
			}
		}

		//printf("Calcul de la matrice C=A*B ...\n");
		#pragma omp parallel for shared(A, B, C) private(i,j,k)
		for (i=0; i<SIZE; i++) 
			for (j=0; j<SIZE; j++) {
				C[i][j] = 0;
				for (k=0; k<SIZE; k++)
					C[i][j] += A[i][k] * B[k][j];
			}
		
	}
	
	texec = omp_get_wtime() - tstart;
	printf("Execution time: %f\n", texec);

	DUMP(stdout, "Show Matrice", C);  
	//showMatrice(C, SIZE);

	// Liberation de memoire
	C = freeMatrice(C, SIZE);
	B = freeMatrice(B, SIZE);
	A = freeMatrice(A, SIZE);

	return 0;
} 
