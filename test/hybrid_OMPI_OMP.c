#include<stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <omp.h>
#include <sched.h>

int main(int argc, char *argv[]){
  int rank, size;
  int nthreads, provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  // omp_set_num_threads(3);
  #pragma omp parallel
    {
    int cpu_num = sched_getcpu();
    printf("ciao, io sono il thread %d con rank_MPI %d on CPU:%d \n", omp_get_thread_num(), rank, cpu_num);

    }

  MPI_Finalize();
  return 0;
}
