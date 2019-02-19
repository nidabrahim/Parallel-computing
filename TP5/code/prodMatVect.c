#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#define SIZE 8		

int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
int X[SIZE], Y[SIZE];

void fill_matrix(int * m, tailleMat_t tm)
{
  static int n=0;
  int i, j;
  for (i=0; i<tm.n; i++)
    for (j=0; j<tm.m; j++)
      m[i][j] = n++;
}

void fill_vector(int * v, tailleMat_t tm)
{
  static int n=0;
  int i;
  for (i=0; i<tm.m; i++)
	v[i] = n++;
}

void print_matrix(int * m, tailleMat_t tm)
{
  int i, j = 0;
  for (i=0; i<tm.n; i++) {
    printf("\n\t| ");
    for (j=0; j<tm.m; j++)
      printf("%2d ", m[i][j]);
    printf("|");
  }
}

void print_vector(int * v, tailleMat_t tm)
{
  int i;
  for (i=0; i<tm.m; i++) {
    printf("\n\t| ");
    printf("%2d ", v[i]);
    printf("|");
  }
}

void Build_derived_type( tailleMat_t *indata_ptr, MPI_Datatype *message_type_ptr )
{
	int blok_lengths[2]; 			/* longueur de chaque champs */
	MPI_Aint displacements[2]; 		/* deplacement memoire de chaque champs par rapport au debut de la structure */
	MPI_Aint addresses[3]; 			/* add. des champs de la struct */
	MPI_Datatype typelist[2];		/* tableau de types des champs */	

	/* Types de chaque membre */
	typelist[0] = typelist[1] = MPI_INT;

	/* Nombre d'elements de chaque membre */
	block_lengths[0]= block_lengths[1] = 1;
	
	/* Obtenir l’add. des ≠ champs d’une structure */
	MPI_Address( indata_ptr, &addresses[0] );
	MPI_Address( &(indata_ptr->n), &addresses[1] );
	MPI_Address( &(indata_ptr->m), &addresses[2] );

	/* Calcul les ecarts en memoire des champs par rapport au debut de la variable */
	displacements[0] = addresses[1] - addresses[0];
	displacements[1] = addresses[2] - addresses[0];

	/* Creation du type derive */
	MPI_Type_struct( 2, block_lengths, displacements, typelist, message_type_ptr );
	MPI_Type_commit( message_type_ptr ); 
}


int main(int argc, char *argv[])
{
  int myrank, nbprocs, from, to, i, j, k;
  int tag = 666;		
  MPI_Status status;
  
  typedef struct {
	int n, m;
  } tailleMat_t;
  
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
	tailleMat_t tailleMat = {4, 4};
	MPI_Datatype TAILLEMAT_TYPE;
	Build_derived_type( &tailleMat, &TAILLEMAT_TYPE );
	MPI_Bcast( &tailleMat, 1, TAILLEMAT_TYPE, 0, MPI_COMM_WORLD );
	
    fill_matrix(A, tailleMat);
    fill_vector(X, tailleMat);
  }

// Diffusion du vecteur X
  MPI_Bcast (X, tailleMat.m, MPI_INT, 0, MPI_COMM_WORLD);
  
// Diffusion des lignes de la matrice A
  MPI_Scatter (A, tailleMat.n*tailleMat.m/nbprocs, MPI_INT, A[from], tailleMat.n*tailleMat.m/nbprocs, MPI_INT, 0, MPI_COMM_WORLD);

// Calcul des Yi
  printf("computing proc %d (Row : %d to %d)\n", myrank, from, to-1);
  for (i=from; i<to; i++) {
	Y[i] = 0;
    for (j=0; j<tailleMat.m; j++)
	   Y[i] += A[i][j]*X[j];
   }

// Fusion des composants de Y
  MPI_Gather (&Y[from], tailleMat.n/nbprocs, MPI_INT, Y, tailleMat.n/nbprocs, MPI_INT, 0, MPI_COMM_WORLD);

// Affichage du résultat
  if (myrank==0) {
    printf("\n\n");
    print_matrix(A, tailleMat);
    printf("\n\n\t\t       * \n");
    print_vector(X, tailleMat);
    printf("\n\n\t       = \n");
    print_vector(Y, tailleMat);
    printf("\n\n");
  }

  MPI_Finalize();
  return 0;
}




