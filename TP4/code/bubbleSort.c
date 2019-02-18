/*
 * Tri a bulle parallele
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mpi.h"

#define LIMIT 100

/*
 * Initialization of a list of integers
 */
void vectorGenInt( int *X, int n )
{
    int i;
    
	srand( time(NULL) );
    for ( i=0; i<n; i++ )
        X[i] = (int) ( ( (double) rand() / RAND_MAX ) * LIMIT );
}

/*
 * Printing of a list of integers on the standard output
 */
void vectorPrintInt( int *X, int n )
{
    int i;
    char ligne[2048]="", tmp[16];
    
    for ( i=0; i<n; i++ ) {
        sprintf(tmp, "%d  ", X[i] );
        strcat(ligne, tmp);
    }
    printf("%s\n", ligne);
}


int main( int argc, char **argv )
{
	int rank=-1, procsNb=0;	/* rank of current proc, number of processors */
	int tag1=50, tag2=150;  /* tags of message                            */
	MPI_Status status;
	
	int *list=NULL;        /* list of elements to sort       */
	int  myElt=-1;         /* element of each processus      */
	int  recvElt=-1;       /* element received from neighbor */
	int  k=0;              /* iteration counter              */

	/* Get basic informations */  
	MPI_Init( &argc, &argv );
	MPI_Comm_rank(  MPI_COMM_WORLD, &rank );
	MPI_Comm_size(  MPI_COMM_WORLD, &procsNb );

	/* Initialization in the Proc 0  */
	if ( !rank ) {
		list = (int *) malloc( procsNb * sizeof(int) );
		if ( !list ) {
			fprintf(stderr, "Proc %d: allocation for list is impossible\n", rank);
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		vectorGenInt(list, procsNb);	

		printf("Initial list : ");
		vectorPrintInt(list, procsNb);
	}

	/* Distribution of list elements */
	MPI_Scatter( list, 1, MPI_INT, &myElt, 1, MPI_INT, 0, MPI_COMM_WORLD);
//  printf("Proc %d : myElt=%d\n", rank, myElt);

	for (k=0; k<=procsNb/2; k++) { 
		if (rank%2==0) { /* even phase */
			if (rank+1 != procsNb) {
				MPI_Send( &myElt, 1, MPI_INT, rank+1, tag1+k, MPI_COMM_WORLD); 
				MPI_Recv( &recvElt, 1, MPI_INT, rank+1, tag2+k, MPI_COMM_WORLD, &status);
				if (recvElt < myElt) myElt = recvElt;

//				printf("k=%d, 1, Proc %d : Send - Recv\n", k, rank);
			}
		}
		else { 
			if (rank != 0) {
				MPI_Recv( &recvElt, 1, MPI_INT, rank-1, tag1+k, MPI_COMM_WORLD, &status);
				MPI_Send( &myElt, 1, MPI_INT, rank-1, tag2+k, MPI_COMM_WORLD); 
				if (recvElt > myElt) myElt = recvElt;

//				printf("k=%d, 1, Proc %d : Recv - Send\n", k, rank);
			}
		}


		if (rank%2) { /* odd phase */
			if (rank+1 != procsNb) {
				MPI_Send( &myElt, 1, MPI_INT, rank+1, tag1+k, MPI_COMM_WORLD); 
				MPI_Recv( &recvElt, 1, MPI_INT, rank+1, tag2+k, MPI_COMM_WORLD, &status);
				if (recvElt < myElt) myElt = recvElt;

//				printf("k=%d, 2, Proc %d : Send - Recv\n", k, rank);
			}
		}
		else { 
			if (rank != 0) {
				MPI_Recv( &recvElt, 1, MPI_INT, rank-1, tag1+k, MPI_COMM_WORLD, &status);
				MPI_Send( &myElt, 1, MPI_INT, rank-1, tag2+k, MPI_COMM_WORLD); 
				if (recvElt > myElt) myElt = recvElt;

//				printf("k=%d, 2, Proc %d : Recv - Send\n", k, rank);
			}
		}
	}

	/* Gather of elements in processes to the Proc 0 */
	MPI_Gather(&myElt, 1, MPI_INT, list, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (!rank) {
		printf("Sorted list  : ");
		vectorPrintInt(list, procsNb);
		free(list);
	}

	MPI_Finalize();
	return EXIT_SUCCESS;
}
