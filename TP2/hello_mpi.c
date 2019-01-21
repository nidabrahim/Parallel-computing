#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


int main(int argc, char **argv)
{
          int rang=-1, nbprocs=0;

          MPI_Init( &argc, &argv );
          MPI_Comm_rank( MPI_COMM_WORLD, &rang );
          MPI_Comm_size( MPI_COMM_WORLD, &nbprocs );

          printf( " Hello from process %d of %d\n ", rang, nbprocs);

          MPI_Finalize();
          return EXIT_SUCCESS;
}
