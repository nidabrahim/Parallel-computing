#include <iostream>
#include <mpi.h>
#include <algorithm>
#include <fstream>
#include <cstring>

using namespace std;


void Make_numbers(long int  big_array[]  /* out */,
		  int       n            /* in  */,
		  int       n_bar        /* in  */,
		  int       p            /* in  */)
{
    int i, q;
    MPI_Status status;

    for (q = 0; q < p; q++) {
      for (i = 0; i < n_bar; i++) {
		big_array[q*n_bar+i] =  random() % (2*n/p) + (q*2*n/p);
      }
    }
} 

void print(int *data, unsigned long data_size)
{
    cout << "Sorted list : ";
    for(unsigned long int i=0; i<data_size; i++)
        cout << data[i] << " ";
    cout << endl;
}

int findPartner(int phase, int rank) {
    int partner;

    /* if it's an even phase */
    if (phase % 2 == 0) {
        /* if we are an even process */
        if (rank % 2 == 0) {
            partner = rank + 1;
        } else {
            partner = rank - 1;
        }
    } else {
        /* it's an odd phase - do the opposite */
        if (rank % 2 == 0) {
            partner = rank - 1;
        } else {
            partner = rank + 1;
        }
    }
    return partner;
}

int compare (const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}

void parallel_sort(int *data, int rank, int count_processes, unsigned long data_size)
{
    const unsigned long concat_data_size = data_size * 2;

    auto *other      = new int[data_size];
    auto *concatData = new int[concat_data_size];

    for (int i=0; i<count_processes/2; i++)
    {
        int partner = findPartner(i, rank);
        if (partner < 0 || partner >= count_processes)
          continue;

        if (rank % 2 == 0) {
            MPI_Send(data, (int) data_size, MPI_INT, partner, 0, MPI_COMM_WORLD);
            MPI_Recv(other, (int) data_size, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(other, (int) data_size, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(data, (int) data_size, MPI_INT, partner, 0, MPI_COMM_WORLD);
        }

        merge(data,  data  + data_size,
              other, other + data_size,
              concatData);
        qsort(concatData, concat_data_size, sizeof(int), compare);

        auto posHalfConcatData = concatData + data_size;
        if (rank < partner)
            copy(concatData, posHalfConcatData, data);
        else
            copy(posHalfConcatData, concatData + concat_data_size, data);
    }
}

/**
 * Compile:      mpic++ OddEvenSort.cpp -o OddEvenSort -std=gnu++0x
 * Example-Call: mpirun -np 4 ./OddEvenSort "<numbers_file.bin>" <y>
 * <y> output on console
 * */
int main(int argCount, char** argValues)
{
    int rank, count_processes;
    int n, n_bar;

    MPI_Init(&argCount, &argValues);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &count_processes);

	long int  * big_array;			/* list of elements to sort       */
	long int  * local_array;		/* elements of each processus     */

	/* Initialization in the Proc 0  */
	if (rank == 0) {
	  if (argCount != 2)
	  {
		 printf("please input the total number of elements to sort: bucketsort_mpi 160000\n");
		 MPI_Finalize();
		 return 1;
	  }
	  n = atoi(argValues[1]);
      big_array = malloc(n*sizeof(long int));
      if (big_array==NULL) {
		 fprintf(stderr, "Big_array malloc failed!!\n");
		 MPI_Abort( MPI_COMM_WORLD, 0 );
		 exit(0);
      }
      printf("\nTotal elements = %d; Each process sorts %d elements.\n", n, n/count_processes);
      Make_numbers(big_array, n, n/count_processes, count_processes);
  }
  
  n_bar = n/count_processes;
  local_array = malloc(n_bar*sizeof(long int));
  if (local_array==NULL) {
      fprintf(stderr, "local_array malloc failed!!\n");
      MPI_Abort( MPI_COMM_WORLD, 0 );
      exit(0);
  }
  /* Distribution of list elements */
  MPI_Scatter(big_array, n_bar, MPI_LONG, local_array, n_bar, MPI_LONG, 0, MPI_COMM_WORLD); 

  parallel_sort(local_array, rank, count_processes, n_bar);
    
  /* Gather of elements in processes to the Proc 0 */
  MPI_Gather(local_array, n_bar, MPI_LONG, local_array, n_bar, MPI_LONG, 0, MPI_COMM_WORLD);

	if (!rank) {
		print(big_array, data_size);
		free(big_array);
	}

    MPI_Finalize();
    return EXIT_SUCCESS;
}
