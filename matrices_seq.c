/*
 *
 */

#include <stdio.h>
#include <stdlib.h>


// taille maximum des matrices statiques sur etud avec 48 threads (par défaut)
// #define SIZE 590 
#define SHOWSIZE 10 

#define DUMP(flux, ch, x) fprintf(flux, "%s %s\n", ch, #x);
  
void showMatrice(double **A, int n)
{
	int i, j;

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

	printf("Initialisation des matrices ...\n");

	/* Initialisation de matrices A et B */
	for (i=0; i<SIZE; i++) {
		for (j=0; j<SIZE; j++) {
			A[i][j] = i*SIZE+j; 
			B[i][j] = i+j; 
		}
	}
	DUMP(stdout, "Show Matrices", AB);  
	showMatrice(A, SIZE);
	showMatrice(B, SIZE);

	printf("Calcul de la matrice C=A*B ...\n");
	for (i=0; i<SIZE; i++) 
		for (j=0; j<SIZE; j++) {
			C[i][j] = 0;
			for (k=0; k<SIZE; k++)
				C[i][j] += A[i][k] * B[k][j];
		}

	DUMP(stdout, "Show Matrice", C);  
	showMatrice(C, SIZE);

	// Liberation de memoire
	C = freeMatrice(C, SIZE);
	B = freeMatrice(B, SIZE);
	A = freeMatrice(A, SIZE);

	return 0;
} 
