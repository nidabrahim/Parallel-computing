#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#define SIZE 8		

int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
int X[SIZE], Y[SIZE];

void fill_matrix(int m[SIZE][SIZE])
{
  static int n=0;
  int i, j;
  for (i=0; i<SIZE; i++)
    for (j=0; j<SIZE; j++)
      m[i][j] = n++;
}

void fill_vector(int v[SIZE])
{
  static int n=0;
  int i;
  for (i=0; i<SIZE; i++)
	v[i] = n++;
}

void print_matrix(int m[SIZE][SIZE])
{
  int i, j = 0;
  for (i=0; i<SIZE; i++) {
    printf("\n\t| ");
    for (j=0; j<SIZE; j++)
      printf("%2d ", m[i][j]);
    printf("|");
  }
}

void print_vector(int v[SIZE])
{
  int i;
  for (i=0; i<SIZE; i++) {
    printf("\n\t| ");
    printf("%2d ", v[i]);
    printf("|");
  }
}


int main(int argc, char *argv[])
{
  int myrank, nbprocs, from, to, i, j, k;
  int tag = 666;		
  MPI_Status status;
  
  MPI_Init (&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);	
  MPI_Comm_size(MPI_COMM_WORLD, &nbprocs); 


  if (SIZE%nbprocs!=0) {
    if (myrank==0) printf("Matrix size not divisible by number of processors\n");
    MPI_Finalize();
    exit(-1);
  }

  from = myrank * SIZE/nbprocs;
  to = (myrank+1) * SIZE/nbprocs;

// Initialisation de la matrice A et du vecteur X
  if (myrank==0) {
    fill_matrix(A);
    fill_vector(X);
  }

// Diffusion du vecteur X
  MPI_Bcast (X, SIZE, MPI_INT, 0, MPI_COMM_WORLD);
  
// Diffusion des lignes de la matrice A
  MPI_Scatter (A, SIZE*SIZE/nbprocs, MPI_INT, A[from], SIZE*SIZE/nbprocs, MPI_INT, 0, MPI_COMM_WORLD);

// Calcul des Yi
  printf("computing proc %d (Row : %d to %d)\n", myrank, from, to-1);
  for (i=from; i<to; i++) {
	Y[i] = 0;
    for (j=0; j<SIZE; j++)
	   Y[i] += A[i][j]*X[j];
   }

// Fusion des composants de Y
  MPI_Gather (&Y[from], SIZE/nbprocs, MPI_INT, Y, SIZE/nbprocs, MPI_INT, 0, MPI_COMM_WORLD);

// Affichage du résultat
  if (myrank==0) {
    printf("\n\n");
    print_matrix(A);
    printf("\n\n\t\t       * \n");
    print_vector(X);
    printf("\n\n\t       = \n");
    print_vector(Y);
    printf("\n\n");
  }

  MPI_Finalize();
  return 0;
}




