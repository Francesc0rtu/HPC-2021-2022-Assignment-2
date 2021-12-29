#include<stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <sched.h>



int main(int argc, char *argv[]){
  int rank, size;
  int nthreads, provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("what a fuck \n");
  MPI_Finalize();
  return 0;
}
