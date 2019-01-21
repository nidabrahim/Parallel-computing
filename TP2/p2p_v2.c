#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char **argv)
{
	int rang, nbprocs, dest=0, source, etiquette = 50;
	MPI_Status statut;
	char message[100];
	char buffer[100];
	
	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rang );
	MPI_Comm_size( MPI_COMM_WORLD, &nbprocs );
	
	if ( rang != 0 ) {
		if( rang == nbprocs-1 ){
			sprintf( message, "Bonjour de la part de P%d" , rang);
		}else{
			MPI_Recv( message, 100, MPI_CHAR, rang+1,
					etiquette, MPI_COMM_WORLD, &statut );
			sprintf( buffer, ", P%d" , rang);
			strcat(message, buffer);
		}
		 MPI_Send( message, strlen(message)+1, MPI_CHAR,
						   rang-1, etiquette, MPI_COMM_WORLD );

	}
	else{
		
		MPI_Recv( message, 100, MPI_CHAR, 1,
					etiquette, MPI_COMM_WORLD, &statut );
		printf( "%s\n", message );
		
	}

	MPI_Finalize();
	return EXIT_SUCCESS ;

}
