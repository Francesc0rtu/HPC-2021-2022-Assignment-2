#include<stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <omp.h>


int main(int argc, char *argv[]){
  int rank, size;
  int nthreads, provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  #pragma omp parallel
    {
    printf("ciao, io sono il thread %d con rank %d \n", omp_get_thread_num(), rank);
    
    }

  MPI_Finalize();
  return 0;
}
