
#include <stdio.h>
#include <stdlib.h>    /* for random function */
#include "mpi.h"

void Make_numbers(long int [], int, int, int);
void Sequential_sort(long int [], int);
int  Get_minpos(long int [], int);
void Put_numbers_in_bucket(long int [], long int [], int, int, int, int);
    
int main(int argc, char* argv[])
{
long int  * big_array;
long int  * local_array;
int       n;
int       n_bar;  /* = n/p */
long int  number;
int       p;
int       my_rank;
int       i;
double    start, stop;   /* for timing */

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &p);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   if (my_rank == 0) {
      if (argc != 2)
      {
         printf("please input the total number of elements to sort: bucketsort_mpi 160000\n");
         MPI_Finalize();
         return 1;
      }
      n = atoi(argv[1]);
/* check if parameters are valid */
      if (n%p != 0) {
		 printf("The number of processes must evenly divide total elements.\n");
		 MPI_Abort( MPI_COMM_WORLD, 0 );
		 MPI_Finalize();
		 return 1;
      } 
      /* make big array */
      start = MPI_Wtime();
      big_array = malloc(n*sizeof(long int));
      if (big_array==NULL) {
	 fprintf(stderr, "Big_array malloc failed!!\n");
	 MPI_Abort( MPI_COMM_WORLD, 0 );
	 exit(0);
      }
      printf("\nTotal elements = %d; Each process sorts %d elements.\n", n, n/p);
      Make_numbers(big_array, n, n/p, p);
   }
   MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
   n_bar = n/p;

   local_array = malloc(n_bar*sizeof(long int));
   if (local_array==NULL) {
      fprintf(stderr, "local_array malloc failed!!\n");
      MPI_Abort( MPI_COMM_WORLD, 0 );
      exit(0);
   }

/*    start = MPI_Wtime();  */
/* Can use scatter if numbers are grouped in the big_array 
       MPI_Scatter(big_array, n_bar, MPI_LONG, local_array, n_bar, MPI_LONG, 0, MPI_COMM_WORLD);  */

   Put_numbers_in_bucket(big_array, local_array, n, n_bar, p, my_rank); 
   Sequential_sort(local_array, n_bar);
   MPI_Gather(local_array, n_bar, MPI_LONG, big_array, n_bar, MPI_LONG, 0, MPI_COMM_WORLD);
   stop = MPI_Wtime();

   if (my_rank==0) {

      for(i=0; i<n; i++) printf("%7ld %c", big_array[i], i%8==7 ? '\n'  : ' ');

      printf("\n\nTime to sort using %d processes = %lf msecs\n", p, (stop - start)/0.001);
   }
   free(local_array);
   if (my_rank==0) free(big_array);
   MPI_Finalize();
}  /* main */
   
/*****************************************************************/
void Make_numbers(long int  big_array[]  /* out */,
		  int       n            /* in  */,
		  int       n_bar        /* in  */,
		  int       p            /* in  */)
{
  /* Puts numbers in "buckets" but we can treat it otherwise  */ 
    int       i, q;
    MPI_Status status;

    for (q = 0; q < p; q++) {
/*
      printf("\nP%d: ", q); */
      for (i = 0; i < n_bar; i++) {
	big_array[q*n_bar+i] =  random() % (2*n/p) + (q*2*n/p);
/*
	printf("%7ld %s", big_array[q*n_bar+i], i%8==7 ? "\n    " : " ");
*/ 
      }
/*      printf("\n");*/
    }
/*    printf("\n");*/
}  /* Make_numbers */

/*****************************************************************/
void Sequential_sort(long int array[]   /* in/out */,
		     int  size          /* in     */)
{
  /* Use selection sort to sort a list from smallest to largest */
   int      eff_size, minpos;
   long int temp;

   for(eff_size = size; eff_size > 1; eff_size--) 
   {
      minpos = Get_minpos(array, eff_size);
      temp = array[minpos];
      array[minpos] = array[eff_size-1];
      array[eff_size-1] = temp;
   }
}

/* Return the index of the smallest element left */
int Get_minpos(long int array[], int eff_size)
{
   int i, minpos = 0;

   for (i=0; i<eff_size; i++)
      minpos = array[i] > array[minpos] ? i: minpos;
   return minpos;
}

/*****************************************************************/
void Put_numbers_in_bucket(long int  big_array[]   /* in */,
			   long int  local_array[] /* out */,
			   int       n             /* in  */,
			   int       n_bar         /* in  */,
			   int       p             /* in  */,
			   int       my_rank       /* in  */) 
{
/* Assume that numbers in big_array are evenly distributed at root,
     but are unsorted.  Send numbers to the process that should have them.  
     This version uses unsafe messaging and may fail in some cases!!   */ 

   int       i, q, bucket;
   MPI_Status status;

   if (my_rank == 0) {
      for (i=0; i<n; i++) {
         bucket = big_array[i]/(2*n_bar);  /* Assume the range is 2*n */
         MPI_Send(&big_array[i], 1, MPI_LONG, bucket, 0, MPI_COMM_WORLD);
         /*      printf("P%d:%ld ", bucket, big_array[i]);     */
      }
   /*    printf("\n"); */
   }
   for (i=0; i<n_bar; i++) {
      MPI_Recv(&local_array[i], 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
   }
}
