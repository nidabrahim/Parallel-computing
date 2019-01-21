#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include "omp.h"

int main()
{

     int tid=-1; char hostname[1024];

     gethostname(hostname, 1024]); tid=omp_get_thread_num();
     printf("Before PARALLEL REGION TID %d: There are %d threads on CPU %d of %s\n\n",

                   tid, omp_get_num_threads(), sched_getcpu(), hostname) ;

     #pragma omp parallel firstprivate(tid)
     {

            tid=omp_get_thread_num();
            if (!tid)

                   printf("In the PARALLEL REGION TID %d: There are %d threads in process\n",
                                omp_get_thread_num(), omp_get_num_threads());

            printf("Hello World from TID %d / %d on CPU %d of %s!\n\n",
                          tid, omp_get_num_threads(), sched_getcpu(), hostname);

     }

     printf("After PARALLEL REGION TID %d: There are %d threads\n\n" ,
                   tid, omp_get_num_threads()) ;

     return EXIT_SUCCESS ;
}
